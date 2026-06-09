#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QEventLoop>

#include "database/sqlitedatabase.h"
#include "database/dbanalyzer.h"
#include "database/queryexecutor.h"
#include "gui/exportorchestrator.h"

class ExportOrchestratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = std::make_unique<QTemporaryDir>();
        tempDir->setAutoRemove(true);
        dbPath = tempDir->path() + "/test.db";
        
        db = std::make_unique<SqliteDatabase>();
        db->setSource(dbPath);
        db->open();

        QStringList createSql;
        createSql << "CREATE TABLE items (id INTEGER PRIMARY KEY, name TEXT, price REAL)";
        QueryExecutor(db.get()).runStatements(createSql);

        QStringList insertSql;
        insertSql << "INSERT INTO items (name, price) VALUES ('Alpha', 10.0)";
        insertSql << "INSERT INTO items (name, price) VALUES ('Beta', 20.0)";
        insertSql << "INSERT INTO items (name, price) VALUES ('Gamma', 30.0)";
        QueryExecutor(db.get()).runStatements(insertSql);

        DbAnalyzer analyzer(db.get());
        analyzer.analyze(info);
    }

    std::unique_ptr<QTemporaryDir> tempDir;
    QString dbPath;
    std::unique_ptr<SqliteDatabase> db;
    DatabaseInfo info;
};

TEST_F(ExportOrchestratorTest, ExportToSqlFile) {
    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    QString filePath = exportDir.path() + "/export.sql";

    auto orchestrator = std::make_unique<ExportOrchestrator>();
    bool completed = false;
    QObject::connect(orchestrator.get(), &ExportOrchestrator::exportCompleted, [&completed]() {
        completed = true;
    });

    orchestrator->exportToSql(info, filePath, db.get());

    // Wait for async export to complete
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, [&loop]() {
        loop.quit();
    });
    QObject::connect(orchestrator.get(), &ExportOrchestrator::exportCompleted, &loop, [&loop]() {
        loop.quit();
    });
    timer.start(5000);
    loop.exec();

    EXPECT_TRUE(QFile::exists(filePath));

    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = QTextStream(&file).readAll();
    file.close();

    EXPECT_TRUE(content.contains("items"));
    EXPECT_TRUE(content.contains("Alpha"));
    EXPECT_TRUE(content.contains("Beta"));
    EXPECT_TRUE(content.contains("Gamma"));
}

TEST_F(ExportOrchestratorTest, ExportToCsvFile) {
    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    QString outputFolder = exportDir.path();

    auto orchestrator = std::make_unique<ExportOrchestrator>();
    orchestrator->exportToCsv(info, outputFolder, ",", db.get());

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, [&loop]() {
        loop.quit();
    });
    QObject::connect(orchestrator.get(), &ExportOrchestrator::exportCompleted, &loop, [&loop]() {
        loop.quit();
    });
    timer.start(5000);
    loop.exec();

    QString csvPath = outputFolder + "/items.csv";
    EXPECT_TRUE(QFile::exists(csvPath));

    QFile file(csvPath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = QTextStream(&file).readAll();
    file.close();

    EXPECT_TRUE(content.contains("id,name,price"));
    EXPECT_TRUE(content.contains("Alpha"));
    EXPECT_TRUE(content.contains("Beta"));
}

TEST_F(ExportOrchestratorTest, CancelExport) {
    // Verify cancel() doesn't crash even when called on a fresh orchestrator
    ExportOrchestrator orchestrator;
    EXPECT_NO_THROW(orchestrator.cancel());
    
    // Also verify it doesn't crash when called after starting an export
    QTemporaryDir exportDir;
    exportDir.setAutoRemove(true);
    QString filePath = exportDir.path() + "/export.sql";
    
    orchestrator.exportToSql(info, filePath, db.get());
    EXPECT_NO_THROW(orchestrator.cancel());
}
