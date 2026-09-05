#include <gtest/gtest.h>
#include <QWidget>
#include <memory>

#include "database/inmemorydatabase.h"
#include "database/queryexecutor.h"
#include "gui/queryexecutionpresenter.h"

class QueryExecutionPresenterTest : public ::testing::Test {
protected:
    void SetUp() override {
        db = std::make_unique<InMemoryDatabase>();
        db->setSource(":memory:");
        db->open();
        executor = std::make_unique<QueryExecutor>(db.get());

        QStringList sql;
        sql << "CREATE TABLE inserted_at (id INTEGER PRIMARY KEY, name TEXT)";
        sql << "INSERT INTO inserted_at (name) VALUES ('a')";
        executor->runStatements(sql);

        parent = std::make_unique<QWidget>();
        presenter = std::make_unique<QueryExecutionPresenter>(parent.get(), executor.get());
    }

    std::unique_ptr<InMemoryDatabase> db;
    std::unique_ptr<QueryExecutor> executor;
    std::unique_ptr<QWidget> parent;
    std::unique_ptr<QueryExecutionPresenter> presenter;
};

TEST_F(QueryExecutionPresenterTest, RunsAScriptAndReportsNoErrors) {
    const ScriptOutcome outcome = presenter->run("SELECT * FROM inserted_at");

    EXPECT_TRUE(outcome.ok());
    EXPECT_TRUE(outcome.errors.isEmpty());
}

TEST_F(QueryExecutionPresenterTest, ReportsTheErrorOfAFailingStatement) {
    const ScriptOutcome outcome = presenter->run("SELECT * FROM nope");

    EXPECT_FALSE(outcome.ok());
    ASSERT_EQ(outcome.errors.size(), 1);
    EXPECT_FALSE(outcome.errors.first().isEmpty());
}

TEST_F(QueryExecutionPresenterTest, SplitsAScriptIntoItsStatements) {
    const ScriptOutcome outcome = presenter->run(
        "SELECT * FROM inserted_at; SELECT nope FROM inserted_at");

    EXPECT_EQ(outcome.errors.size(), 1);
}

TEST_F(QueryExecutionPresenterTest, ReportsASchemaChangeForCreate) {
    const ScriptOutcome outcome = presenter->run("CREATE TABLE fresh (id INTEGER)");

    EXPECT_TRUE(outcome.schemaChanged);
}

// ALTER changes the Schema but was not in the keyword list the window used to
// match on, so the Tree silently went stale after one.
TEST_F(QueryExecutionPresenterTest, ReportsASchemaChangeForAlter) {
    const ScriptOutcome outcome = presenter->run("ALTER TABLE inserted_at ADD COLUMN extra TEXT");

    EXPECT_TRUE(outcome.schemaChanged);
}

// The bug this deepening was for: the window matched "insert" anywhere in the
// statement text, so selecting from a table whose name contains it re-analysed
// the database and forced the messages tab open.
TEST_F(QueryExecutionPresenterTest, DoesNotReportASchemaChangeForASelectNamingADdlWord) {
    const ScriptOutcome outcome = presenter->run("SELECT * FROM inserted_at");

    EXPECT_FALSE(outcome.schemaChanged);
}

TEST_F(QueryExecutionPresenterTest, DoesNotReportASchemaChangeForAnInsert) {
    const ScriptOutcome outcome = presenter->run("INSERT INTO inserted_at (name) VALUES ('b')");

    EXPECT_FALSE(outcome.schemaChanged);
}

// Reading the statement text would call this a schema change; SQLite knows it
// was not.
TEST_F(QueryExecutionPresenterTest, DoesNotReportASchemaChangeForARolledBackCreate) {
    const ScriptOutcome outcome = presenter->run(
        "BEGIN; CREATE TABLE rolled (id INTEGER); ROLLBACK");

    EXPECT_FALSE(outcome.schemaChanged);
}

TEST_F(QueryExecutionPresenterTest, TimesTheRun) {
    const ScriptOutcome outcome = presenter->run("SELECT * FROM inserted_at");

    EXPECT_GE(outcome.elapsedMs, 0);
}
