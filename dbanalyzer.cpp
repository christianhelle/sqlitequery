#include "dbanalyzer.h"

DatabaseAnalyzer::DatabaseAnalyzer()
{
    qDebug("DatabaseAnalyzer::DatabaseAnalyzer()");

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setHostName("localhost");
}

DatabaseAnalyzer::~DatabaseAnalyzer()
{
    qDebug("DatabaseAnalyzer::~DatabaseAnalyzer()");

    this->close();
}

void DatabaseAnalyzer::close()
{
    qDebug("DatabaseAnalyzer::close()");

    if (database.isOpen())
        database.close();
}

bool DatabaseAnalyzer::open(QString filename)
{
    qDebug("DatabaseAnalyzer::open(QString)");

    this->close();
    this->filename = filename;
    database.setDatabaseName(filename);
    return database.open();
}

bool DatabaseAnalyzer::analyze(DatabaseInfo &info)
{
    QFileInfo file(this->filename);

    info.filename = file.fileName();
    info.size = file.size();
    info.creationDate = file.created();

    return true;
}

