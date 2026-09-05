#include "dbexportschema.h"

#include <QFile>

#include "sqlidentifier.h"

QString DbSchemaExport::exportSchema() const {
    QStringList createTableScripts;
    for (const auto &table: this->getDatabaseInfo().tables) {
        if (isInternalTable(table)) {
            continue;
        }
        QString createTableScript = QString("CREATE TABLE %1 (").arg(quotedIdentifier(table.name));
        QStringList columnDefinitions;
        for (const auto &column: table.columns) {
            // The Column name is an Identifier; the declared type is not,
            // and delimiting it would break a type like VARCHAR(50).
            QString columnDefinition = QString("\n  %1 %2").arg(
                quotedIdentifier(column.name), column.dataType);
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

void DbSchemaExport::exportSchemaToFile(const QString &filename) const {
    const auto sql = this->exportSchema();
    if (sql.isEmpty())
        return;
    const auto file = std::make_unique<QFile>(filename);
    if (file->
        open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(file.get());
        out << sql;
        file->close();
    }
}
