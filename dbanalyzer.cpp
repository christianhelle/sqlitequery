#include "dbanalyzer.h"

DbAnalyzer::DbAnalyzer()
{
    qDebug("DatabaseAnalyzer::DatabaseAnalyzer()");

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setHostName("localhost");
}

DbAnalyzer::~DbAnalyzer()
{
    qDebug("DatabaseAnalyzer::~DatabaseAnalyzer()");

    this->close();
}

void DbAnalyzer::close()
{
    qDebug("DatabaseAnalyzer::close()");

    if (database.isOpen())
        database.close();
}

void DbAnalyzer::shrink()
{
    if (!database.isOpen())
        database.open();

    QSqlQuery query(database);
    query.exec("VACUUM");
}

bool DbAnalyzer::open(QString filename)
{
    qDebug("DatabaseAnalyzer::open(QString)");

    this->close();
    this->filename = filename;
    database.setDatabaseName(filename);
    return database.open();
}

bool DbAnalyzer::analyze(DatabaseInfo &info)
{
    QFileInfo file(this->filename);

    info.filename = file.fileName();
    info.size = file.size();
    info.creationDate = file.created();

    loadTables(info);
    loadColumns(info);

    return true;
}

void DbAnalyzer::loadTables(DatabaseInfo &info)
{
    const QString sql = "SELECT * FROM sqlite_master WHERE type='table'";
    qDebug() << sql;

    QSqlQuery query(this->database);
    query.exec(sql);

    while (query.next())
    {
        Table table;
        table.name = query.value("name").toString();
        info.tables.append(table);
        qDebug() << table.name;
    }
}

void DbAnalyzer::loadColumns(DatabaseInfo &info)
{
    foreach (Table table, info.tables)
    {
        const QString sql = "PRAGMA table_info (" + table.name + ")";
        qDebug() << sql;

        QSqlQuery query(this->database);
        query.exec(sql);

        while (query.next())
        {
            Column col;
            col.ordinal = query.value("cid").toInt();
            col.name = query.value("name").toString();
            col.dataType = query.value("type").toString();
            col.notNull = query.value("notnull").toBool();
            table.columns.append(col);
            qDebug() << col.ordinal << col.name << col.dataType << col.notNull;
        }
    }
}
