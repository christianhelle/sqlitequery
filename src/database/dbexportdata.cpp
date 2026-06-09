#include "dbexportdata.h"
#include <QFile>

QStringList DbDataExport::getColumnDefs(const Table &table) {
    QStringList columnDefinitions;
    for (const auto &column: table.columns) {
        columnDefinitions.append(column.name);
    }
    return columnDefinitions;
}

QStringList DbDataExport::getColumnValueDefs(const Table &table,
                                             const QList<QVariant> &values) const {
    QStringList valueDefinitions;
    for (int i = 0; i < table.columns.size(); ++i) {
        const auto &column = table.columns.at(i);
        auto value = values.at(i).toString();
        bool isText = false;
        for (const auto &type: getTextTypes()) {
            if (column.dataType.contains(type, Qt::CaseInsensitive)) {
                isText = true;
                break;
            }
        }
        if (isText) {
            value = value.replace("\"", "\"\"");
            valueDefinitions.append(QString("\"%1\"").arg(value));
        } else {
            valueDefinitions.append(value);
        }
    }
    return valueDefinitions;
}

void DbDataExport::exportDataToSqlFile(IDatabase *database,
                                       const QString &filename,
                                       const CancellationToken *cancellationToken,
                                       ExportDataProgress *progress) const {
    const auto file = std::make_unique<QFile>(filename);
    if (!file->
        open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return;
    }

    progress->reset();
    QTextStream out(file.get());
    for (const auto &table: this->getDatabaseInfo().tables) {
        if (isInternalTable(table)) {
            continue;
        }
        out << "-- " << table.name << "\n";

        const auto columns = getColumnDefs(table).join(", ");
        const QueryResult streamResult = database->streamRows(
            QString("SELECT * FROM \"%1\"").arg(table.name),
            [&](const QList<QVariant> &values) {
                if (cancellationToken->isCancellationRequested())
                    return false;
                const auto valueList = getColumnValueDefs(table, values).join(", ");
                out << "INSERT INTO \"" << table.name << "\"(" << columns << ") ";
                out << "VALUES (" << valueList << ");\n";
                progress->increment();
                return true;
            });

        if (!streamResult.ok) {
            continue;
        }
        out << "\n";
    }
    file->close();
    progress->setCompleted();
}

void DbDataExport::exportDataToCsvFile(IDatabase *database,
                                       const QString &outputFolder,
                                       const QString &delimiter,
                                       const CancellationToken *cancellationToken,
                                       ExportDataProgress *progress) const {
    progress->reset();
    for (const auto &table: this->getDatabaseInfo().tables) {
        if (isInternalTable(table) || cancellationToken->isCancellationRequested()) {
            continue;
        }

        const auto filename = outputFolder + "/" + table.name + ".csv";
        const auto file = std::make_unique<QFile>(filename);
        if (!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            return;
        }

        const auto columns = getColumnDefs(table).join(delimiter);
        QTextStream out(file.get());
        out << columns << "\n";

        const QueryResult streamResult = database->streamRows(
            QString("SELECT * FROM \"%1\"").arg(table.name),
            [&](const QList<QVariant> &values) {
                if (cancellationToken->isCancellationRequested())
                    return false;
                const auto valueList = getColumnValueDefs(table, values).join(delimiter);
                out << valueList << "\n";
                progress->increment();
                return true;
            });

        file->close();
        if (!streamResult.ok) {
            continue;
        }
    }

    progress->setCompleted();
}
