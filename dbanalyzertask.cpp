#include "dbanalyzertask.h"

DbAnalyzerTask::DbAnalyzerTask(Database *database, DbTree *tree)
{
    this->database = database;
    this->tree = tree;
}

DbAnalyzerTask::~DbAnalyzerTask()
{
    qDebug("DbAnalyzerTask::~DbAnalyzerTask()");
}

void DbAnalyzerTask::run()
{
    qDebug("MainWindow::analyzeDatabase()");

    DatabaseInfo info;
    DbAnalyzer analyzer(this->database);
    if (!analyzer.analyze(info))
    {
        qDebug("Analyze database failed");
        return;
    }

    this->tree->populateTree(info);
    this->database->close();
}

