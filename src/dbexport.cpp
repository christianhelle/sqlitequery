#include "dbexport.h"

#include <QFile>

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

void DbExport::exportSchemaToFile(const QString &filename) const {
    const auto sql = this->exportSchema();
    if (sql.isEmpty())
        return;
    const auto file = std::make_unique<QFile>(filename);
    if (file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(file.get());
        out << sql;
        file->close();
    }
}

void DbExport::exportDataToFile(const Database * database, const QString &filename) const {
    const auto file = std::make_unique<QFile>(filename);
    if (file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(file.get());
        for (const auto &table: this->info.tables) {
            if (isInternalTable(table)) {
                continue;
            }
            out << "-- " << table.name << "\n";

            QSqlQuery query(database->getDatabase());
            query.exec(QString("SELECT * FROM %1").arg(table.name));
            while (query.next()) {
                out << "INSERT INTO " << table.name << "(";

                QStringList columnDefinitions;
                for (const auto &column: table.columns) {
                    // if (column.primaryKey)
                    //     continue;
                    columnDefinitions.append(column.name);
                }
                out << columnDefinitions.join(", ") << ") VALUES (";

                QMap<QString, QString> values;
                for (const auto &column: table.columns) {
                    // if (column.primaryKey)
                    //     continue;
                    const auto value = query.value(column.name).toString();
                    values[value] = column.dataType;
                }
                QStringList valueDefinitions;
                for (const auto &value: values.keys()) {
                    bool isText = false;
                    for (const auto type: textTypes) {
                        if (values[value].contains(type, Qt::CaseInsensitive)) {
                            isText = true;
                            break;
                        }
                    }
                    if (isText) {
                        valueDefinitions.append(QString("\"%1\"").arg(value));
                    } else {
                        valueDefinitions.append(value);
                    }
                }
                out << valueDefinitions.join(", ") << ");\n";
            }
            out << "\n";
        }
        file->close();
    }
}

bool DbExport::isInternalTable(const Table &table) {
    if (table.name == "sqlite_sequence" || table.name == "sqlite_stat1") {
        return true;
    }
    return false;
}
