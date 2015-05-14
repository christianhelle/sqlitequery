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

    loadTables(info);
    loadColumns(info);

    return true;
}

void DatabaseAnalyzer::loadTables(DatabaseInfo &info)
{
    QSqlQuery query(this->database);
    query.exec("SELECT * FROM sqlite_master WHERE type='table'");

    while (query.next())
    {
        Table table;
        table.name = query.value("name").toString();
        info.tables.append(table);
        qDebug(table.name.toStdString().c_str());
    }
}

void DatabaseAnalyzer::loadColumns(DatabaseInfo &info)
{
    foreach (Table table, info.tables)
    {
        QSqlQuery query(this->database);
        query.exec("PRAGMA table_info (" + table.name + ")");

        while (query.next())
        {
            Column col;
            col.ordinal = query.value("cid").toInt();
            col.name = query.value("name").toString();
            col.dataType = query.value("type").toString();
            col.allowNull = query.value("notnull").toBool();
            table.columns.append(col);
        }
    }
}

