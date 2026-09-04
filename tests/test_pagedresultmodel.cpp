#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QAbstractItemModel>

#include "database/sqlitedatabase.h"
#include "database/queryexecutor.h"

class PagedResultTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = std::make_unique<QTemporaryDir>();
        dbPath = tempDir->path() + "/paged.db";
        db = std::make_unique<SqliteDatabase>();
        db->setSource(dbPath);
        db->open();

        QStringList sql;
        sql << "CREATE TABLE big (id INTEGER PRIMARY KEY, name TEXT)";
        sql << "BEGIN TRANSACTION";
        for (int i = 0; i < RowCount; ++i)
            sql << QString("INSERT INTO big (name) VALUES ('name%1')").arg(i);
        sql << "COMMIT";
        QueryExecutor(db.get()).runStatements(sql);
    }

    static constexpr int RowCount = 50000;

    std::unique_ptr<QTemporaryDir> tempDir;
    QString dbPath;
    std::unique_ptr<SqliteDatabase> db;
};

// The whole point: opening a large result set must not read all of it.
TEST_F(PagedResultTest, DoesNotFetchEveryRowUpFront) {
    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("SELECT * FROM big"));

    ASSERT_NE(model, nullptr);
    EXPECT_EQ(model->columnCount(), 2);
    EXPECT_GT(model->rowCount(), 0);
    EXPECT_LT(model->rowCount(), RowCount) << "the entire result set was loaded up front";
    EXPECT_TRUE(model->canFetchMore(QModelIndex()));
}

// ...and scrolling must be able to reach every row, however many there are.
TEST_F(PagedResultTest, FetchesEveryRowOnDemand) {
    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("SELECT * FROM big"));
    ASSERT_NE(model, nullptr);

    while (model->canFetchMore(QModelIndex()))
        model->fetchMore(QModelIndex());

    EXPECT_EQ(model->rowCount(), RowCount);
    EXPECT_EQ(model->data(model->index(0, 1)).toString(), "name0");
    EXPECT_EQ(model->data(model->index(RowCount - 1, 1)).toString(),
              QString("name%1").arg(RowCount - 1));
}

TEST_F(PagedResultTest, SortsInTheDatabase) {
    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("SELECT * FROM big"));
    ASSERT_NE(model, nullptr);

    model->sort(0, Qt::DescendingOrder);

    // Sorted by the database, so the first row is the last id, even though only
    // the first page has been fetched.
    EXPECT_LT(model->rowCount(), RowCount);
    EXPECT_EQ(model->data(model->index(0, 0)).toInt(), RowCount);
}

TEST_F(PagedResultTest, ReportsErrorForInvalidStatement) {
    QString error;
    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("this is not sql at all", &error));

    EXPECT_EQ(model, nullptr);
    EXPECT_FALSE(error.isEmpty());
}

TEST_F(PagedResultTest, ReturnsNoModelForStatementWithoutRows) {
    QString error;
    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("INSERT INTO big (name) VALUES ('extra')", &error));

    EXPECT_EQ(model, nullptr);
    EXPECT_TRUE(error.isEmpty()) << error.toStdString();

    // The statement still ran.
    const QueryResult count = db->runStatement("SELECT COUNT(*) FROM big");
    EXPECT_EQ(count.rows.at(0).values.at(0).toInt(), RowCount + 1);
}
