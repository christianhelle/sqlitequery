#include "dbanalyzer.h"

DbAnalyzer::DbAnalyzer(Database *database)
{
    qDebug("DatabaseAnalyzer::DatabaseAnalyzer()");
    this->database = database;
}

DbAnalyzer::~DbAnalyzer()
{
    qDebug("DatabaseAnalyzer::~DatabaseAnalyzer()");
}

bool DbAnalyzer::analyze(DatabaseInfo &info)
{
    QFileInfo file(this->database->getFilename());

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

    QSqlQuery query(this->database->getDatabase());
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

        QSqlQuery query(this->database->getDatabase());
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
