#include "dbexport.h"

QString DbExport::exportSchema() const {
    QStringList createTableScripts;
    for (const auto &table: this->info.tables) {
        if (isInternalTable(table)) {
            continue;
        }
        QString createTableScript = QString("CREATE TABLE %1 (").arg(table.name);
        QStringList columnDefinitions;
        for (const auto &column: table.columns) {
            QString columnDefinition = QString("\n  %1 %2").arg(column.name).arg(column.dataType);
            if (column.primaryKey) {
                columnDefinition += " PRIMARY KEY";
            }
            if (column.notNull) {
                columnDefinition += " NOT NULL";
            }
            columnDefinitions.append(columnDefinition);
        }
        createTableScript += columnDefinitions.join(",") + "\n);";
        createTableScripts.append(createTableScript);
    }

    return createTableScripts.join("\n\n");
}

bool DbExport::isInternalTable(const Table &table) {
    if (table.name == "sqlite_sequence" || table.name == "sqlite_stat1") {
        return true;
    }
    return false;
}
