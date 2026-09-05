#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QAbstractItemModel>

#include "database/sqlitedatabase.h"
#include "database/dbanalyzer.h"
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

// Multi-line SQL is how anyone writes a query in the editor; the newlines must
// survive, and a `--` comment must not swallow the rest of the statement.
TEST_F(PagedResultTest, RunsMultiLineStatements) {
    QueryExecutor executor(db.get());
    QStringList statements;
    statements << "SELECT id, name\nFROM big\nWHERE id <= 10";
    QStringList errors;

    const auto models = executor.runStatementsPaged(statements, &errors);

    EXPECT_TRUE(errors.isEmpty()) << errors.join("; ").toStdString();
    ASSERT_EQ(models.size(), 1);
    EXPECT_EQ(models.at(0)->columnCount(), 2);
    qDeleteAll(models);
}

TEST_F(PagedResultTest, KeepsStatementsAfterALineComment) {
    QueryExecutor executor(db.get());
    QStringList statements;
    statements << "SELECT id -- the identifier\nFROM big";
    QStringList errors;

    const auto models = executor.runStatementsPaged(statements, &errors);

    EXPECT_TRUE(errors.isEmpty()) << errors.join("; ").toStdString();
    ASSERT_EQ(models.size(), 1);
    qDeleteAll(models);
}

// A table name may legitimately contain a double quote, and it reaches the
// query as an identifier, so it has to be escaped rather than interpolated.
TEST_F(PagedResultTest, PreviewsATableWhoseNameContainsAQuote) {
    QueryExecutor executor(db.get());
    QStringList create;
    create << "CREATE TABLE \"we\"\"ird\" (id INTEGER PRIMARY KEY)";
    create << "INSERT INTO \"we\"\"ird\" (id) VALUES (7)";
    executor.runStatements(create);

    QString error;
    const std::unique_ptr<QAbstractItemModel> model(
        executor.previewTablePaged("we\"ird", &error));

    ASSERT_NE(model, nullptr) << error.toStdString();
    EXPECT_EQ(model->rowCount(), 1);
    EXPECT_EQ(model->data(model->index(0, 0)).toInt(), 7);
}

// Sorting a statement that cannot be wrapped in a subquery must not re-run it.
TEST_F(PagedResultTest, DoesNotReExecuteAStatementItCannotSort) {
    const QueryResult before = db->runStatement("SELECT COUNT(*) FROM big");
    const int rowsBefore = before.rows.at(0).values.at(0).toInt();

    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("INSERT INTO big (name) VALUES ('sorted') RETURNING id"));
    ASSERT_NE(model, nullptr) << "RETURNING should produce a result set";

    model->sort(0, Qt::AscendingOrder);

    const QueryResult after = db->runStatement("SELECT COUNT(*) FROM big");
    EXPECT_EQ(after.rows.at(0).values.at(0).toInt(), rowsBefore + 1)
        << "the statement ran a second time while sorting";
}

TEST_F(PagedResultTest, ClearsStaleErrorOnSuccess) {
    QString error = "left over from an earlier call";

    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("SELECT * FROM big", &error));

    EXPECT_NE(model, nullptr);
    EXPECT_TRUE(error.isEmpty()) << error.toStdString();
}

TEST_F(PagedResultTest, ClearsStaleErrorWhenThereIsNoResultSet) {
    QString error = "left over from an earlier call";

    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("INSERT INTO big (name) VALUES ('x')", &error));

    EXPECT_EQ(model, nullptr);
    EXPECT_TRUE(error.isEmpty()) << error.toStdString();
}

// A PagedResult reads its remaining pages from the connection it was built
// with, so anything that closes that connection cuts the result short with no
// error -- the rows simply stop. Analysing is the way that used to happen:
// executeQuery re-analyses whenever it thinks the Schema changed, and the
// Analyzer closed the connection partway through.
TEST_F(PagedResultTest, SurvivesAnAnalyzeWhileItIsStillBeingScrolled) {
    const std::unique_ptr<QAbstractItemModel> reference(
        db->createResultModel("SELECT * FROM big"));
    ASSERT_NE(reference, nullptr);
    while (reference->canFetchMore(QModelIndex()))
        reference->fetchMore(QModelIndex());
    const int everyRow = reference->rowCount();
    ASSERT_EQ(everyRow, RowCount);

    const std::unique_ptr<QAbstractItemModel> model(
        db->createResultModel("SELECT * FROM big"));
    ASSERT_NE(model, nullptr);

    DatabaseInfo info;
    DbAnalyzer(db.get()).analyze(info);

    while (model->canFetchMore(QModelIndex()))
        model->fetchMore(QModelIndex());

    EXPECT_EQ(model->rowCount(), everyRow);
    EXPECT_EQ(model->data(model->index(everyRow - 1, 1)).toString(),
              QString("name%1").arg(RowCount - 1));
}
