//
// Created by Christian Helle on 28/03/2025.
//

#include "dbschemaexport.h"

#include <QFile>

QString DbSchemaExport::exportSchema() const
{
    QStringList createTableScripts;
    for (const auto& table : this->info.tables)
    {
        if (isInternalTable(table))
        {
            continue;
        }
        QString createTableScript = QString("CREATE TABLE %1 (").arg(table.name);
        QStringList columnDefinitions;
        for (const auto& column : table.columns)
        {
            QString columnDefinition = QString("\n  %1 %2").arg(column.name, column.dataType);
            if (column.primaryKey)
            {
                columnDefinition += " PRIMARY KEY";
            }
            if (column.notNull)
            {
                columnDefinition += " NOT NULL";
            }
            columnDefinitions.append(columnDefinition);
        }
        createTableScript += columnDefinitions.join(",") + "\n);";
        createTableScripts.append(createTableScript);
    }

    return createTableScripts.join("\n\n");
}

void DbSchemaExport::exportSchemaToFile(const QString& filename) const
{
    const auto sql = this->exportSchema();
    if (sql.isEmpty())
        return;
    const auto file = std::make_unique<QFile>(filename);
    if (file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(file.get());
        out << sql;
        file->close();
    }
}
