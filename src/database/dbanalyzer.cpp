#include "dbanalyzer.h"

#include <QFileInfo>

DbAnalyzer::DbAnalyzer(Database *database)
    : database(database) {
    qDebug("DatabaseAnalyzer::DatabaseAnalyzer()");
}

bool DbAnalyzer::analyze(DatabaseInfo &info) const {
    const QFileInfo file(this->database->getFilename());

    info.filename = file.fileName();
    info.size = file.size();
    info.creationDate = file.birthTime();

    loadTables(info);
    loadColumns(info);

    return true;
}

void DbAnalyzer::loadTables(DatabaseInfo &info) const {
    const QString sql = "SELECT * FROM sqlite_master WHERE type='table'";
    qDebug() << sql;

    if (!this->database->open()) {
        qDebug("Unable to open database");
        return;
    }

    QSqlQuery query(this->database->getDatabase());
    query.exec(sql);

    while (query.next()) {
        Table table;
        table.name = query.value("name").toString();
        if (table.name == "sqlite_sequence" || table.name == "sqlite_stat1") {
            continue;
        }
        info.tables.append(table);
        qDebug() << table.name;
    }

    database->close();
}

void DbAnalyzer::loadColumns(DatabaseInfo &info) const {
    if (!this->database->open()) {
        qDebug("Unable to open database");
        return;
    }

    for (auto &table: info.tables) {
        const QString sql = "PRAGMA table_info (" + table.name + ")";
        qDebug() << sql;

        QSqlQuery query(this->database->getDatabase());
        query.exec(sql);

        while (query.next()) {
            Column col;
            col.ordinal = query.value("cid").toInt();
            col.name = query.value("name").toString();
            col.dataType = query.value("type").toString();
            col.notNull = query.value("notnull").toBool();
            col.defaultValue = query.value("dflt_value").toString();
            col.primaryKey = query.value("pk").toBool();
            table.columns.append(col);
            qDebug() << col.ordinal << col.name << col.dataType
                    << col.notNull << col.defaultValue << col.primaryKey;
        }
    }
}
