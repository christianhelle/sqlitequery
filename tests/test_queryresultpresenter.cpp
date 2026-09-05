#include <gtest/gtest.h>
#include <QAbstractItemModel>
#include <QApplication>
#include <QHeaderView>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QTableView>
#include <QWidget>
#include <algorithm>

#include "gui/queryresultpresenter.h"

namespace {
    // Records what the view asked the model to sort by. A paged result answers
    // a sort by re-running its statement, so an unasked-for sort is both the
    // wrong order and a second execution of the query.
    class SortSpyModel final : public QStandardItemModel {
    public:
        SortSpyModel(const int rows, const int columns)
            : QStandardItemModel(rows, columns) {
        }

        void sort(const int column, const Qt::SortOrder order) override {
            this->sortedColumns.append(column);
            this->sortedOrders.append(order);
            QStandardItemModel::sort(column, order);
        }

        // Sorting by column -1 is Qt's "leave it in its natural order", so it
        // does not count as having sorted anything.
        [[nodiscard]] bool wasSorted() const {
            return std::any_of(sortedColumns.begin(),
                               sortedColumns.end(),
                               [](const int column) { return column >= 0; });
        }

        QList<int> sortedColumns{};
        QList<Qt::SortOrder> sortedOrders{};
    };

    void clickHeaderSection(const QTableView *view, const int section) {
        auto *header = view->horizontalHeader();
        const QPoint position(
            header->sectionViewportPosition(section) + header->sectionSize(section) / 2,
            header->viewport()->height() / 2);
        const QPoint global = header->viewport()->mapToGlobal(position);
        QMouseEvent press(QEvent::MouseButtonPress, position, global,
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QMouseEvent release(QEvent::MouseButtonRelease, position, global,
                            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(header->viewport(), &press);
        QApplication::sendEvent(header->viewport(), &release);
    }
}

class QueryResultPresenterTest : public ::testing::Test {
protected:
    void SetUp() override {
        parent = std::make_unique<QWidget>();
        parent->resize(800, 600);
        parent->show();
        presenter = std::make_unique<QueryResultPresenter>(parent.get());
    }

    // Stands in for a PagedResult; the presenter only ever binds models.
    static SortSpyModel *modelWithRows(const int rows) {
        auto *model = new SortSpyModel(rows, 2);
        for (int row = 0; row < rows; ++row) {
            model->setItem(row, 0, new QStandardItem(QString::number(row)));
            model->setItem(row, 1, new QStandardItem("name" + QString::number(row)));
        }
        return model;
    }

    std::unique_ptr<QWidget> parent;
    std::unique_ptr<QueryResultPresenter> presenter;
};

TEST_F(QueryResultPresenterTest, PresentShowsResults) {
    presenter->present({modelWithRows(3)});

    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 1);
    ASSERT_NE(views.at(0)->model(), nullptr);
    EXPECT_EQ(views.at(0)->model()->rowCount(), 3);
    EXPECT_TRUE(views.at(0)->isVisible());
}

TEST_F(QueryResultPresenterTest, PresentShowsOneViewPerResult) {
    presenter->present({modelWithRows(3), modelWithRows(5)});

    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 2);
}

TEST_F(QueryResultPresenterTest, PresentDiscardsPreviousResults) {
    for (int i = 1; i <= 4; ++i) {
        presenter->present({modelWithRows(i)});

        const auto views = parent->findChildren<QTableView *>();
        ASSERT_EQ(views.size(), 1) << "views from execution " << i - 1 << " were left behind";
        ASSERT_NE(views.at(0)->model(), nullptr);
        EXPECT_EQ(views.at(0)->model()->rowCount(), i);
    }
}

TEST_F(QueryResultPresenterTest, PresentKeepsResultsWhenNothingToRender) {
    presenter->present({modelWithRows(3)});
    presenter->present({});   // e.g. an INSERT, which displays nothing

    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 1) << "a statement that renders nothing cleared the results";
    ASSERT_NE(views.at(0)->model(), nullptr);
    EXPECT_EQ(views.at(0)->model()->rowCount(), 3);
}

TEST_F(QueryResultPresenterTest, PresentTakesOwnershipOfModels) {
    auto *model = modelWithRows(3);
    presenter->present({model});

    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 1);
    EXPECT_EQ(model->parent(), views.at(0)) << "model is not owned by its view";
}

TEST_F(QueryResultPresenterTest, PresentToViewReplacesModel) {
    QTableView view(parent.get());

    for (int i = 1; i <= 4; ++i) {
        presenter->presentToView(&view, modelWithRows(i));

        ASSERT_NE(view.model(), nullptr) << "preview " << i;
        EXPECT_EQ(view.model()->rowCount(), i) << "preview " << i;
        EXPECT_EQ(view.model()->parent(), &view) << "preview " << i;
    }
}

TEST_F(QueryResultPresenterTest, PresentToViewIgnoresMissingModel) {
    QTableView view(parent.get());
    presenter->presentToView(&view, modelWithRows(3));

    presenter->presentToView(&view, nullptr);

    ASSERT_NE(view.model(), nullptr) << "a failed preview cleared the view";
    EXPECT_EQ(view.model()->rowCount(), 3);
}

// Qt's header starts out on the first column in descending order, and enabling
// sorting sorts the bound model straight away. A paged result answers that by
// re-running its statement wrapped in an ORDER BY, which both reverses the rows
// and makes the database order the whole result set before the first page can
// be shown.
TEST_F(QueryResultPresenterTest, PresentDoesNotSortTheResult) {
    auto *model = modelWithRows(3);

    presenter->present({model});

    EXPECT_FALSE(model->wasSorted()) << "the result was sorted before anyone asked for an order";

    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 1);
    EXPECT_EQ(views.at(0)->horizontalHeader()->sortIndicatorSection(), -1)
        << "the result opened claiming to be sorted by a column";
}

TEST_F(QueryResultPresenterTest, PresentToViewDoesNotSortTheTable) {
    QTableView view(parent.get());

    auto *first = modelWithRows(3);
    presenter->presentToView(&view, first);
    auto *second = modelWithRows(4);
    presenter->presentToView(&view, second);

    EXPECT_FALSE(first->wasSorted()) << "the outgoing table was sorted on its way out";
    EXPECT_FALSE(second->wasSorted()) << "the table was sorted before anyone asked for an order";
}

TEST_F(QueryResultPresenterTest, FirstClickOnAColumnSortsAscending) {
    auto *model = modelWithRows(3);
    presenter->present({model});
    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 1);

    clickHeaderSection(views.at(0), 0);

    ASSERT_FALSE(model->sortedOrders.isEmpty()) << "clicking the header sorted nothing";
    EXPECT_EQ(model->sortedColumns.last(), 0);
    EXPECT_EQ(model->sortedOrders.last(), Qt::AscendingOrder);
}

TEST_F(QueryResultPresenterTest, SecondClickOnTheSameColumnSortsDescending) {
    auto *model = modelWithRows(3);
    presenter->present({model});
    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 1);

    clickHeaderSection(views.at(0), 0);
    clickHeaderSection(views.at(0), 0);

    EXPECT_EQ(model->sortedOrders.last(), Qt::DescendingOrder);
}
