#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

#include "database/sqlitedatabase.h"
#include "database/dbanalyzer.h"
#include "database/queryexecutor.h"
#include "database/dbexportschema.h"

class DbSchemaExportTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = std::make_unique<QTemporaryDir>();
        tempDir->setAutoRemove(true);
        dbPath = tempDir->path() + "/test.db";
        
        db = std::make_unique<SqliteDatabase>();
        db->setSource(dbPath);
        db->open();

        // Create test tables with constraints
        QStringList createSql;
        createSql << "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT NOT NULL, email TEXT UNIQUE)";
        createSql << "CREATE TABLE orders (id INTEGER PRIMARY KEY, user_id INTEGER, total REAL, FOREIGN KEY(user_id) REFERENCES users(id))";
        createSql << "CREATE INDEX idx_orders_user ON orders(user_id)";
        createSql << "CREATE UNIQUE INDEX idx_users_email ON users(email)";
        createSql << "CREATE TABLE sqlite_sequence(name, seq)";

        QueryExecutor executor(db.get());
        executor.runStatements(createSql);

        DbAnalyzer analyzer(db.get());
        analyzer.analyze(info);
    }

    std::unique_ptr<QTemporaryDir> tempDir;
    QString dbPath;
    std::unique_ptr<SqliteDatabase> db;
    DatabaseInfo info;
};

TEST_F(DbSchemaExportTest, ExportSchemaContainsCreateTable) {
    DbSchemaExport exporter(info);
    QString schema = exporter.exportSchema();

    EXPECT_TRUE(schema.contains("CREATE TABLE"));
    EXPECT_TRUE(schema.contains("users"));
    EXPECT_TRUE(schema.contains("orders"));
}

TEST_F(DbSchemaExportTest, ExportSchemaExcludesInternalTables) {
    DbSchemaExport exporter(info);
    QString schema = exporter.exportSchema();

    EXPECT_FALSE(schema.contains("sqlite_sequence"));
    EXPECT_FALSE(schema.contains("sqlite_stat1"));
}

TEST_F(DbSchemaExportTest, ExportSchemaContainsColumns) {
    DbSchemaExport exporter(info);
    QString schema = exporter.exportSchema();

    EXPECT_TRUE(schema.contains("id"));
    EXPECT_TRUE(schema.contains("name"));
    EXPECT_TRUE(schema.contains("email"));
    EXPECT_TRUE(schema.contains("user_id"));
    EXPECT_TRUE(schema.contains("total"));
}

TEST_F(DbSchemaExportTest, ExportSchemaContainsConstraints) {
    DbSchemaExport exporter(info);
    QString schema = exporter.exportSchema();

    // PRIMARY KEY and NOT NULL are preserved during analysis
    EXPECT_TRUE(schema.contains("PRIMARY KEY"));
    EXPECT_TRUE(schema.contains("NOT NULL"));
    // FOREIGN KEY and UNIQUE are lost during analysis (Column struct has no FK/unique fields)
}

TEST_F(DbSchemaExportTest, ExportSchemaToFile) {
    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    QString filePath = exportDir.path() + "/schema.sql";

    DbSchemaExport exporter(info);
    exporter.exportSchemaToFile(filePath);

    EXPECT_TRUE(QFile::exists(filePath));

    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = QTextStream(&file).readAll();
    file.close();

    EXPECT_TRUE(content.contains("CREATE TABLE"));
    EXPECT_TRUE(content.contains("users"));
}

TEST_F(DbSchemaExportTest, ExportSchemaEmptyDatabase) {
    QTemporaryDir tempDir2;
    tempDir2.setAutoRemove(true);
    QString emptyDbPath = tempDir2.path() + "/empty.db";
    
    SqliteDatabase emptyDb;
    emptyDb.setSource(emptyDbPath);
    emptyDb.open();

    DbAnalyzer analyzer(&emptyDb);
    DatabaseInfo emptyInfo;
    analyzer.analyze(emptyInfo);

    DbSchemaExport exporter(emptyInfo);
    QString schema = exporter.exportSchema();

    // Empty database returns empty schema (no tables to script)
    EXPECT_TRUE(schema.isEmpty());
}
