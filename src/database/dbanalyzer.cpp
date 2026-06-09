#include "dbanalyzer.h"

#include <QFileInfo>

DbAnalyzer::DbAnalyzer(IDatabase *database)
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

    const bool wasOpen = this->database->isOpen();
    if (!wasOpen && !this->database->open()) {
        return;
    }

    const QueryResult result = this->database->runStatement(sql);
    if (!result.ok) {
        if (!wasOpen) database->close();
        return;
    }

    const int nameIdx = result.columns.indexOf("name");
    for (const auto &row: result.rows) {
        const QString name = row.values.at(nameIdx).toString();
        if (name == "sqlite_sequence" || name == "sqlite_stat1") {
            continue;
        }
        Table table;
        table.name = name;
        info.tables.append(table);
    }

    if (!wasOpen) database->close();
}

void DbAnalyzer::loadColumns(DatabaseInfo &info) const {
    if (!this->database->open()) {
        return;
    }

    for (auto &table: info.tables) {
        const QString sql = "PRAGMA table_info (\"" + table.name + "\")";

        const QueryResult result = this->database->runStatement(sql);
        if (!result.ok) {
            continue;
        }

        const int ordinalIdx = result.columns.indexOf("cid");
        const int nameIdx = result.columns.indexOf("name");
        const int typeIdx = result.columns.indexOf("type");
        const int notNullIdx = result.columns.indexOf("notnull");
        const int defaultIdx = result.columns.indexOf("dflt_value");
        const int pkIdx = result.columns.indexOf("pk");

        for (const auto &row: result.rows) {
            Column col;
            col.ordinal = row.values.at(ordinalIdx).toInt();
            col.name = row.values.at(nameIdx).toString();
            col.dataType = row.values.at(typeIdx).toString();
            col.notNull = row.values.at(notNullIdx).toBool();
            col.defaultValue = row.values.at(defaultIdx).toString();
            col.primaryKey = row.values.at(pkIdx).toBool();
            table.columns.append(col);
        }
    }
}
