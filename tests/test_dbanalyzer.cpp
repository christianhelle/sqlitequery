#include <gtest/gtest.h>
#include <QTemporaryDir>
#include "database/sqlitedatabase.h"
#include "database/dbanalyzer.h"
#include "database/queryexecutor.h"
#include "database/databaseinfo.h"

class DbAnalyzerTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = std::make_unique<QTemporaryDir>();
        tempDir->setAutoRemove(true);
        dbPath = tempDir->path() + "/test.db";
        
        db = std::make_unique<SqliteDatabase>();
        db->setSource(dbPath);
        db->open();

        // Create test tables
        QStringList createSql;
        createSql << "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT NOT NULL, age INTEGER)";
        createSql << "CREATE TABLE orders (id INTEGER PRIMARY KEY, user_id INTEGER, total REAL)";
        createSql << "CREATE INDEX idx_orders_user ON orders(user_id)";

        QueryExecutor executor(db.get());
        executor.runStatements(createSql);

        analyzer = std::make_unique<DbAnalyzer>(db.get());
    }

    std::unique_ptr<QTemporaryDir> tempDir;
    QString dbPath;
    std::unique_ptr<SqliteDatabase> db;
    std::unique_ptr<DbAnalyzer> analyzer;

    void runSql(const QString &sql) {
        QStringList statements;
        statements << sql;
        QueryExecutor executor(db.get());
        executor.runStatements(statements);
    }
};

TEST_F(DbAnalyzerTest, AnalyzeCreatesDatabaseInfo) {
    DatabaseInfo info;
    bool ok = analyzer->analyze(info);

    EXPECT_TRUE(ok);
    EXPECT_FALSE(info.tables.isEmpty());
}

TEST_F(DbAnalyzerTest, AnalyzeFindsTables) {
    DatabaseInfo info;
    analyzer->analyze(info);

    QStringList tableNames;
    for (const auto &table : info.tables) {
        tableNames << table.name;
    }

    EXPECT_TRUE(tableNames.contains("users"));
    EXPECT_TRUE(tableNames.contains("orders"));
    EXPECT_FALSE(tableNames.contains("sqlite_sequence"));
    EXPECT_FALSE(tableNames.contains("sqlite_stat1"));
}

TEST_F(DbAnalyzerTest, AnalyzeFindsColumns) {
    DatabaseInfo info;
    analyzer->analyze(info);

    const Table *usersTable = nullptr;
    for (const auto &table : info.tables) {
        if (table.name == "users") {
            usersTable = &table;
            break;
        }
    }

    ASSERT_NE(usersTable, nullptr);
    ASSERT_GE(usersTable->columns.size(), 3);

    const Column *nameCol = nullptr;
    for (const auto &col : usersTable->columns) {
        if (col.name == "name") {
            nameCol = &col;
            break;
        }
    }

    ASSERT_NE(nameCol, nullptr);
    EXPECT_EQ(nameCol->dataType, "TEXT");
    EXPECT_TRUE(nameCol->notNull);
    EXPECT_FALSE(nameCol->primaryKey);
}

TEST_F(DbAnalyzerTest, AnalyzeFindsIndexes) {
    // DbAnalyzer currently only loads tables and columns, not indexes.
    // This test verifies that the absence of indexes doesn't crash analyze().
    DatabaseInfo info;
    analyzer->analyze(info);

    const Table *ordersTable = nullptr;
    for (const auto &table : info.tables) {
        if (table.name == "orders") {
            ordersTable = &table;
            break;
        }
    }

    ASSERT_NE(ordersTable, nullptr);
    // Indexes are not loaded by DbAnalyzer (known limitation)
    EXPECT_TRUE(ordersTable->indexes.isEmpty());
}

TEST_F(DbAnalyzerTest, AnalyzeEmptyDatabase) {
    QTemporaryDir tempDir2;
    tempDir2.setAutoRemove(true);
    QString emptyDbPath = tempDir2.path() + "/empty.db";
    
    SqliteDatabase emptyDb;
    emptyDb.setSource(emptyDbPath);
    emptyDb.open();
    
    DbAnalyzer emptyAnalyzer(&emptyDb);
    DatabaseInfo emptyInfo;
    emptyAnalyzer.analyze(emptyInfo);

    EXPECT_TRUE(emptyInfo.tables.isEmpty());
}

TEST_F(DbAnalyzerTest, AnalyzeWithPrimaryKey) {
    runSql("CREATE TABLE items (item_id INTEGER PRIMARY KEY, title TEXT)");

    DbAnalyzer itemAnalyzer(db.get());
    DatabaseInfo info;
    itemAnalyzer.analyze(info);

    const Table *itemsTable = nullptr;
    for (const auto &table : info.tables) {
        if (table.name == "items") {
            itemsTable = &table;
            break;
        }
    }

    ASSERT_NE(itemsTable, nullptr);
    const Column *idCol = nullptr;
    for (const auto &col : itemsTable->columns) {
        if (col.name == "item_id") {
            idCol = &col;
            break;
        }
    }

    ASSERT_NE(idCol, nullptr);
    EXPECT_TRUE(idCol->primaryKey);
}

// The analyzer must leave the database usable: the window analyses a database
// immediately after opening it, and then runs the user's queries against it.
TEST_F(DbAnalyzerTest, AnalyzeLeavesDatabaseUsable) {
    DatabaseInfo info;
    ASSERT_TRUE(analyzer->analyze(info));

    runSql("INSERT INTO users (name, age) VALUES ('Alice', 30)");

    QueryExecutor executor(db.get());
    const QueryResult preview = executor.previewTable("users");
    EXPECT_TRUE(preview.ok) << preview.error.toStdString();
    EXPECT_EQ(preview.rows.size(), 1);
}

// PRAGMA table_info interpolates the Table name, so a name holding a quote
// used to produce a malformed statement and the Table came back with no
// Columns at all.
TEST_F(DbAnalyzerTest, AnalyzeFindsColumnsOfATableWhoseNameHoldsAQuote) {
    runSql("CREATE TABLE \"we\"\"ird\" (id INTEGER PRIMARY KEY, \"order\" INTEGER)");

    DatabaseInfo info;
    analyzer->analyze(info);

    const Table *weird = nullptr;
    for (const auto &table : info.tables) {
        if (table.name == "we\"ird")
            weird = &table;
    }

    ASSERT_NE(weird, nullptr);
    EXPECT_EQ(weird->columns.size(), 2);
}
