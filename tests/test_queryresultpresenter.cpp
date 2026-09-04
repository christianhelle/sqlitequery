#include <gtest/gtest.h>
#include <QAbstractItemModel>
#include <QStandardItemModel>
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

    // Stands in for a PagedResult; the presenter only ever binds models.
    static QAbstractItemModel *modelWithRows(const int rows) {
        auto *model = new QStandardItemModel(rows, 2);
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
