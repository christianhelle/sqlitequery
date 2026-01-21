#include "dbanalyzer.h"

#include <QFileInfo>

DbAnalyzer::DbAnalyzer(Database *database)
    : database(database) {
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

    if (!this->database->open()) {
        return;
    }

    QSqlQuery query(this->database->getDatabase());
    if (!query.exec(sql)) {
        database->close();
        return;
    }

    while (query.next()) {
        Table table;
        table.name = query.value("name").toString();
        if (table.name == "sqlite_sequence" || table.name == "sqlite_stat1") {
            continue;
        }
        info.tables.append(table);
    }

    database->close();
}

void DbAnalyzer::loadColumns(DatabaseInfo &info) const {
    if (!this->database->open()) {
        return;
    }

    for (auto &table: info.tables) {
        const QString sql = "PRAGMA table_info (\"" + table.name + "\")";

        QSqlQuery query(this->database->getDatabase());
        if (!query.exec(sql)) {
            continue;
        }

        while (query.next()) {
            Column col;
            col.ordinal = query.value("cid").toInt();
            col.name = query.value("name").toString();
            col.dataType = query.value("type").toString();
            col.notNull = query.value("notnull").toBool();
            col.defaultValue = query.value("dflt_value").toString();
            col.primaryKey = query.value("pk").toBool();
            table.columns.append(col);
        }
    }
}
