#include "dbanalyzer.h"

DbAnalyzer::DbAnalyzer(Database *database) {
    qDebug("DatabaseAnalyzer::DatabaseAnalyzer()");
    this->database = database;
}

DbAnalyzer::~DbAnalyzer() {
    qDebug("DatabaseAnalyzer::~DatabaseAnalyzer()");
}

bool DbAnalyzer::analyze(DatabaseInfo &info) const {
    QFileInfo file(this->database->getFilename());

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
        info.tables.append(table);
        qDebug() << table.name;
    }
}

void DbAnalyzer::loadColumns(DatabaseInfo &info) const {
    for (int i = 0; i < info.tables.length(); ++i) {
        const QString sql = "PRAGMA table_info (" + info.tables[i].name + ")";
        qDebug() << sql;

        QSqlQuery query(this->database->getDatabase());
        query.exec(sql);

        while (query.next()) {
            Column col;
            col.ordinal = query.value("cid").toInt();
            col.name = query.value("name").toString();
            col.dataType = query.value("type").toString();
            col.notNull = query.value("notnull").toBool();
            info.tables[i].columns.append(col);
            qDebug() << col.ordinal << col.name << col.dataType << col.notNull;
        }
    }
}
