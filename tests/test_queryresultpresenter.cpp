#include <gtest/gtest.h>
#include <QAbstractItemModel>
#include <QTableView>
#include <QWidget>

#include "gui/queryresultpresenter.h"

class QueryResultPresenterTest : public ::testing::Test {
protected:
    void SetUp() override {
        parent = std::make_unique<QWidget>();
        parent->resize(800, 600);
        parent->show();
        presenter = std::make_unique<QueryResultPresenter>(parent.get());
    }

    static QueryResult selectResult(const int rows) {
        QueryResult result;
        result.ok = true;
        result.isSelect = true;
        result.columns << "id" << "name";
        for (int i = 0; i < rows; ++i) {
            QueryRow row;
            row.values << QVariant(i) << QVariant("name" + QString::number(i));
            result.rows.append(row);
        }
        return result;
    }

    std::unique_ptr<QWidget> parent;
    std::unique_ptr<QueryResultPresenter> presenter;
};

TEST_F(QueryResultPresenterTest, PresentShowsResults) {
    presenter->present({selectResult(3)});

    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 1);
    ASSERT_NE(views.at(0)->model(), nullptr);
    EXPECT_EQ(views.at(0)->model()->rowCount(), 3);
    EXPECT_EQ(views.at(0)->model()->columnCount(), 2);
    EXPECT_TRUE(views.at(0)->isVisible());
}

TEST_F(QueryResultPresenterTest, PresentDiscardsPreviousResults) {
    for (int i = 1; i <= 4; ++i) {
        presenter->present({selectResult(i)});

        const auto views = parent->findChildren<QTableView *>();
        ASSERT_EQ(views.size(), 1) << "views from execution " << i - 1 << " were left behind";
        ASSERT_NE(views.at(0)->model(), nullptr);
        EXPECT_EQ(views.at(0)->model()->rowCount(), i);
    }
}

TEST_F(QueryResultPresenterTest, PresentKeepsResultsWhenNothingToRender) {
    presenter->present({selectResult(3)});

    QueryResult notASelect;
    notASelect.ok = true;
    notASelect.isSelect = false;
    presenter->present({notASelect});

    const auto views = parent->findChildren<QTableView *>();
    ASSERT_EQ(views.size(), 1) << "a statement that renders nothing cleared the results";
    ASSERT_NE(views.at(0)->model(), nullptr);
    EXPECT_EQ(views.at(0)->model()->rowCount(), 3);
}

TEST_F(QueryResultPresenterTest, PresentToViewReplacesModel) {
    QTableView view(parent.get());

    for (int i = 1; i <= 4; ++i) {
        presenter->presentToView(&view, selectResult(i));

        ASSERT_NE(view.model(), nullptr) << "preview " << i;
        EXPECT_EQ(view.model()->rowCount(), i) << "preview " << i;
        EXPECT_EQ(view.model()->columnCount(), 2) << "preview " << i;
        EXPECT_EQ(view.model()->parent(), &view) << "preview " << i;
    }
}
