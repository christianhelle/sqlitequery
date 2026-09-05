#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

#include "database/sqlitedatabase.h"
#include "database/dbanalyzer.h"
#include "database/queryexecutor.h"
#include "database/dbexportdata.h"
#include "threading/cancellation.h"

class DbDataExportTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = std::make_unique<QTemporaryDir>();
        tempDir->setAutoRemove(true);
        dbPath = tempDir->path() + "/test.db";
        
        db = std::make_unique<SqliteDatabase>();
        db->setSource(dbPath);
        db->open();

        // Create test data
        QStringList createSql;
        createSql << "CREATE TABLE products (id INTEGER PRIMARY KEY, name TEXT, price REAL)";
        createSql << "CREATE TABLE sqlite_sequence(name, seq)"; // Internal table

        QueryExecutor executor(db.get());
        executor.runStatements(createSql);

        QStringList insertSql;
        insertSql << "INSERT INTO products (name, price) VALUES ('Widget', 9.99)";
        insertSql << "INSERT INTO products (name, price) VALUES ('Gadget', 24.99)";
        insertSql << "INSERT INTO products (name, price) VALUES ('Doohickey', 4.50)";
        executor.runStatements(insertSql);

        // Analyze to get DatabaseInfo
        DbAnalyzer analyzer(db.get());
        analyzer.analyze(info);
    }

    std::unique_ptr<QTemporaryDir> tempDir;
    QString dbPath;
    std::unique_ptr<SqliteDatabase> db;
    DatabaseInfo info;

    void runSql(const QStringList &statements) const {
        QueryExecutor(db.get()).runStatements(statements);
    }

    // Re-reads the Schema, for a test that has changed it since SetUp.
    [[nodiscard]] DatabaseInfo reanalyze() const {
        DatabaseInfo fresh;
        DbAnalyzer(db.get()).analyze(fresh);
        return fresh;
    }

    // Exports a Schema as an INSERT script and returns what was written.
    [[nodiscard]] QString exportedSqlScript(const DatabaseInfo &schema) const {
        QTemporaryDir exportDir;
        exportDir.setAutoRemove(true);
        const QString path = exportDir.path() + "/export.sql";

        const DbDataExport exporter(schema);
        CancellationTokenSource tcs;
        const CancellationToken token = tcs.get();
        ExportDataProgress progress;
        exporter.exportDataToSqlFile(db.get(), path, &token, &progress);

        QFile file(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QString content = QTextStream(&file).readAll();
        file.close();
        return content;
    }
};

TEST_F(DbDataExportTest, ExportToSqlFile) {
    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    QString filePath = exportDir.path() + "/export.sql";

    DbDataExport exporter(info);
    CancellationTokenSource tcs;
    CancellationToken token = tcs.get();
    ExportDataProgress progress;

    exporter.exportDataToSqlFile(db.get(), filePath, &token, &progress);

    EXPECT_TRUE(progress.isCompleted());
    EXPECT_TRUE(QFile::exists(filePath));

    // Verify file content
    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = QTextStream(&file).readAll();
    file.close();

    EXPECT_TRUE(content.contains("products"));
    EXPECT_TRUE(content.contains("Widget"));
    EXPECT_TRUE(content.contains("Gadget"));
    EXPECT_TRUE(content.contains("Doohickey"));
    EXPECT_FALSE(content.contains("sqlite_sequence"));
}

TEST_F(DbDataExportTest, ExportToCsvFile) {
    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    QString outputFolder = exportDir.path();

    DbDataExport exporter(info);
    CancellationTokenSource tcs;
    CancellationToken token = tcs.get();
    ExportDataProgress progress;

    exporter.exportDataToCsvFile(db.get(), outputFolder, ",", &token, &progress);

    EXPECT_TRUE(progress.isCompleted());

    // Verify CSV files exist
    QString csvPath = outputFolder + "/products.csv";
    EXPECT_TRUE(QFile::exists(csvPath));

    QFile file(csvPath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = QTextStream(&file).readAll();
    file.close();

    EXPECT_TRUE(content.contains("id,name,price"));
    EXPECT_TRUE(content.contains("Widget"));
    EXPECT_TRUE(content.contains("Gadget"));
}

TEST_F(DbDataExportTest, ExportWithTextTypes) {
    // Use fresh temp dir to avoid file conflicts
    QTemporaryDir testDir;
    testDir.setAutoRemove(true);
    QString testDbPath = testDir.path() + "/test.db";
    
    SqliteDatabase testDb;
    testDb.setSource(testDbPath);
    testDb.open();

    QStringList createSql;
    createSql << "CREATE TABLE strings (id INTEGER PRIMARY KEY, text_col TEXT, varchar_col VARCHAR, character_col CHARACTER)";
    QueryExecutor(&testDb).runStatements(createSql);

    QStringList insertSql;
    insertSql << "INSERT INTO strings (text_col, varchar_col, character_col) VALUES ('hello', 'world', 'test')";
    QueryExecutor(&testDb).runStatements(insertSql);

    DbAnalyzer analyzer(&testDb);
    DatabaseInfo testInfo;
    analyzer.analyze(testInfo);

    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    QString csvPath = exportDir.path() + "/strings.csv";

    DbDataExport exporter(testInfo);
    CancellationTokenSource tcs;
    CancellationToken token = tcs.get();
    ExportDataProgress progress;

    exporter.exportDataToCsvFile(&testDb, exportDir.path(), ",", &token, &progress);

    QFile file(csvPath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = QTextStream(&file).readAll();
    file.close();

    // Text types (TEXT, VARCHAR, CHARACTER) should be quoted
    EXPECT_TRUE(content.contains("\"hello\""));
    EXPECT_TRUE(content.contains("\"world\""));
    EXPECT_TRUE(content.contains("\"test\""));
}

TEST_F(DbDataExportTest, ExportWithSpecialCharacters) {
    // Use fresh temp dir to avoid file conflicts
    QTemporaryDir testDir;
    testDir.setAutoRemove(true);
    QString testDbPath = testDir.path() + "/test.db";
    
    SqliteDatabase testDb;
    testDb.setSource(testDbPath);
    testDb.open();

    QStringList createSql;
    createSql << "CREATE TABLE special (id INTEGER PRIMARY KEY, data TEXT)";
    QueryExecutor(&testDb).runStatements(createSql);

    QStringList insertSql;
    insertSql << "INSERT INTO special (data) VALUES ('He said \"hello\"')";
    QueryExecutor(&testDb).runStatements(insertSql);

    DbAnalyzer analyzer(&testDb);
    DatabaseInfo testInfo;
    analyzer.analyze(testInfo);

    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    QString sqlPath = exportDir.path() + "/special.sql";

    DbDataExport exporter(testInfo);
    CancellationTokenSource tcs;
    CancellationToken token = tcs.get();
    ExportDataProgress progress;

    exporter.exportDataToSqlFile(&testDb, sqlPath, &token, &progress);

    QFile file(sqlPath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = QTextStream(&file).readAll();
    file.close();

    // Quotes should be escaped
    EXPECT_TRUE(content.contains("\"\"\""));
}

TEST_F(DbDataExportTest, ExportRowsCounted) {
    DbDataExport exporter(info);
    CancellationTokenSource tcs;
    CancellationToken token = tcs.get();
    ExportDataProgress progress;

    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    exporter.exportDataToSqlFile(db.get(), exportDir.path() + "/export.sql", &token, &progress);

    EXPECT_EQ(progress.getAffectedRows(), 3u); // 3 products
}

// The SELECT that reads a table for export interpolates its name, and the
// INSERT it writes interpolates it again, so a name holding a quote used to
// export nothing at all.
TEST_F(DbDataExportTest, ExportsATableWhoseNameHoldsAQuote) {
    runSql({R"(CREATE TABLE "we""ird" (id INTEGER PRIMARY KEY, name TEXT))",
            R"(INSERT INTO "we""ird" (name) VALUES ('row'))"});

    const QString content = exportedSqlScript(reanalyze());

    EXPECT_TRUE(content.contains(R"(INSERT INTO "we""ird")"));
}

// The column list of an INSERT is SQL, so a Column named after a reserved
// word has to be delimited or the script will not replay.
TEST_F(DbDataExportTest, DelimitsColumnNamesInTheInsertColumnList) {
    runSql({R"(CREATE TABLE reserved (id INTEGER PRIMARY KEY, "order" INTEGER))",
            R"(INSERT INTO reserved ("order") VALUES (1))"});

    const QString content = exportedSqlScript(reanalyze());

    EXPECT_TRUE(content.contains(R"("order")"));
}

// A CSV header is not SQL. The Column names go in as the user wrote them, so
// delimiting the INSERT column list must not follow them here.
TEST_F(DbDataExportTest, CsvHeaderKeepsColumnNamesUndelimited) {
    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);

    DbDataExport exporter(info);
    CancellationTokenSource tcs;
    CancellationToken token = tcs.get();
    ExportDataProgress progress;
    exporter.exportDataToCsvFile(db.get(), exportDir.path(), ",", &token, &progress);

    QFile file(exportDir.path() + "/products.csv");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    const QString header = QTextStream(&file).readLine();
    file.close();

    EXPECT_EQ(header, "id,name,price");
}
