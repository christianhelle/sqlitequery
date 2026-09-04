#include <gtest/gtest.h>
#include "database/inmemorydatabase.h"
#include "database/queryexecutor.h"
#include "database/queryresult.h"

class QueryExecutorTest : public ::testing::Test {
protected:
    void SetUp() override {
        db = std::make_unique<InMemoryDatabase>();
        db->setSource(":memory:");
        db->open();
        executor = std::make_unique<QueryExecutor>(db.get());

        // Create test table
        QStringList createSql;
        createSql << "CREATE TABLE test_users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)";
        executor->runStatements(createSql);

        // Insert test data
        QStringList insertSql;
        insertSql << "INSERT INTO test_users (name, age) VALUES ('Alice', 30)";
        insertSql << "INSERT INTO test_users (name, age) VALUES ('Bob', 25)";
        insertSql << "INSERT INTO test_users (name, age) VALUES ('Charlie', 35)";
        executor->runStatements(insertSql);
    }

    std::unique_ptr<InMemoryDatabase> db;
    std::unique_ptr<QueryExecutor> executor;
};

TEST_F(QueryExecutorTest, RunSelectQuery) {
    QStringList statements;
    statements << "SELECT * FROM test_users";
    auto results = executor->runStatements(statements);

    ASSERT_EQ(results.size(), 1);
    EXPECT_TRUE(results[0].ok);
    EXPECT_TRUE(results[0].isSelect);
    EXPECT_EQ(results[0].columns.size(), 3);
    EXPECT_EQ(results[0].rows.size(), 3);
}

TEST_F(QueryExecutorTest, RunInsertQuery) {
    QStringList statements;
    statements << "INSERT INTO test_users (name, age) VALUES ('Dave', 40)";
    auto results = executor->runStatements(statements);

    ASSERT_EQ(results.size(), 1);
    EXPECT_TRUE(results[0].ok);
    EXPECT_FALSE(results[0].isSelect);
    EXPECT_EQ(results[0].rowsAffected, 1);
}

TEST_F(QueryExecutorTest, RunScript) {
    QString script = "SELECT COUNT(*) as count FROM test_users";
    auto results = executor->runScript(script);

    ASSERT_EQ(results.size(), 1);
    EXPECT_TRUE(results[0].ok);
    EXPECT_TRUE(results[0].isSelect);
    EXPECT_EQ(results[0].rows.size(), 1);
}

TEST_F(QueryExecutorTest, RunInvalidQuery) {
    QStringList statements;
    statements << "SELECT * FROM nonexistent_table";
    QStringList errors;
    auto results = executor->runStatements(statements, &errors);

    ASSERT_EQ(results.size(), 1);
    EXPECT_FALSE(results[0].ok);
    EXPECT_FALSE(results[0].error.isEmpty());
    EXPECT_GT(errors.size(), 0);
}

// Arbitrary text is not valid SQL and must be reported, not silently ignored.
TEST_F(QueryExecutorTest, RunNonSqlTextReportsError) {
    QStringList statements;
    statements << "this is not sql at all";
    QStringList errors;
    const auto results = executor->runStatements(statements, &errors);

    ASSERT_EQ(results.size(), 1);
    EXPECT_FALSE(results.at(0).ok);
    ASSERT_EQ(errors.size(), 1);
    EXPECT_FALSE(errors.at(0).isEmpty());
}

TEST_F(QueryExecutorTest, RunMultipleQueries) {
    QStringList statements;
    statements << "SELECT COUNT(*) FROM test_users"
               << "SELECT name FROM test_users WHERE age > 30";
    auto results = executor->runStatements(statements);

    ASSERT_EQ(results.size(), 2);
    EXPECT_TRUE(results[0].ok);
    EXPECT_TRUE(results[1].ok);
}

TEST_F(QueryExecutorTest, RunEmptyScript) {
    QString emptyScript = "   \n  \t  ";
    auto results = executor->runScript(emptyScript);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(QueryExecutorTest, RunMixedValidInvalidQueries) {
    QStringList statements;
    statements << "SELECT COUNT(*) FROM test_users"
               << "SELECT * FROM nonexistent"
               << "SELECT name FROM test_users";
    QStringList errors;
    auto results = executor->runStatements(statements, &errors);

    ASSERT_EQ(results.size(), 3);
    EXPECT_TRUE(results[0].ok);
    EXPECT_FALSE(results[1].ok);
    EXPECT_TRUE(results[2].ok);
}

TEST_F(QueryExecutorTest, PreviewTable) {
    QueryResult result = executor->previewTable("test_users");

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.isSelect);
    EXPECT_EQ(result.rows.size(), 3);
    EXPECT_EQ(result.columns.size(), 3);
    EXPECT_TRUE(result.columns.contains("id"));
    EXPECT_TRUE(result.columns.contains("name"));
    EXPECT_TRUE(result.columns.contains("age"));
}

TEST_F(QueryExecutorTest, PreviewTableWithLimit) {
    QueryResult result = executor->previewTable("test_users", 2);

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.isSelect);
    EXPECT_EQ(result.rows.size(), 2);
}

// Browsing a large table must not pull the whole table into memory.
TEST_F(QueryExecutorTest, PreviewTableCapsLargeTables) {
    QStringList rows;
    rows << "BEGIN TRANSACTION";
    for (int i = 0; i < 20000; ++i) {
        rows << QString("INSERT INTO test_users (name, age) VALUES ('user%1', %1)").arg(i);
    }
    rows << "COMMIT";
    executor->runStatements(rows);

    const QueryResult unlimited = executor->previewTable("test_users");
    EXPECT_GT(unlimited.rows.size(), 1000);

    const QueryResult limited = executor->previewTable("test_users", 1000);
    EXPECT_TRUE(limited.ok);
    EXPECT_EQ(limited.rows.size(), 1000);
}

// A row cap must bound what a statement reads, without rewriting the SQL.
TEST_F(QueryExecutorTest, RunStatementHonoursRowCap) {
    const QueryResult capped = db->runStatement("SELECT * FROM test_users", 2);

    EXPECT_TRUE(capped.ok);
    EXPECT_TRUE(capped.isSelect);
    EXPECT_EQ(capped.rows.size(), 2);
    EXPECT_TRUE(capped.truncated);
    EXPECT_EQ(capped.columns.size(), 3);
}

TEST_F(QueryExecutorTest, RunStatementIsNotTruncatedWhenCapNotReached) {
    const QueryResult capped = db->runStatement("SELECT * FROM test_users", 100);

    EXPECT_EQ(capped.rows.size(), 3);
    EXPECT_FALSE(capped.truncated);
}

TEST_F(QueryExecutorTest, RunStatementIsNotTruncatedWhenCapEqualsRowCount) {
    const QueryResult capped = db->runStatement("SELECT * FROM test_users", 3);

    EXPECT_EQ(capped.rows.size(), 3);
    EXPECT_FALSE(capped.truncated);
}

TEST_F(QueryExecutorTest, RunStatementWithoutCapReadsEverything) {
    const QueryResult all = db->runStatement("SELECT * FROM test_users");

    EXPECT_EQ(all.rows.size(), 3);
    EXPECT_FALSE(all.truncated);
}

// The query editor runs arbitrary SQL through runStatements; an unbounded
// SELECT must still come back bounded, and say that it was cut short.
TEST_F(QueryExecutorTest, RunStatementsHonoursRowCap) {
    QStringList rows;
    rows << "BEGIN TRANSACTION";
    for (int i = 0; i < 20000; ++i) {
        rows << QString("INSERT INTO test_users (name, age) VALUES ('user%1', %1)").arg(i);
    }
    rows << "COMMIT";
    executor->runStatements(rows);

    QStringList statements;
    statements << "SELECT * FROM test_users";
    QStringList errors;
    const auto results = executor->runStatements(statements, &errors, 1000);

    ASSERT_EQ(results.size(), 1);
    EXPECT_TRUE(errors.isEmpty());
    EXPECT_TRUE(results.at(0).ok);
    EXPECT_EQ(results.at(0).rows.size(), 1000);
    EXPECT_TRUE(results.at(0).truncated);
}

TEST_F(QueryExecutorTest, PreviewNonexistentTable) {
    QueryResult result = executor->previewTable("nonexistent_table");

    EXPECT_FALSE(result.ok);
}
