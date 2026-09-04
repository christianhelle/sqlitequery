#include "dbexportdata.h"
#include <QFile>

QStringList DbDataExport::getColumnDefs(const Table &table) {
    QStringList columnDefinitions;
    for (const auto &column : table.columns) {
        columnDefinitions.append(column.name);
    }
    return columnDefinitions;
}

// Whether a column holds text depends only on the schema, so this is resolved
// once per table rather than once per exported cell.
QList<bool> DbDataExport::getTextColumnFlags(const Table &table) const {
    QList<bool> isTextColumn;
    isTextColumn.reserve(table.columns.size());
    for (const auto &column : table.columns) {
        bool isText = false;
        for (const auto &type : getTextTypes()) {
            if (column.dataType.contains(type, Qt::CaseInsensitive)) {
                isText = true;
                break;
            }
        }
        isTextColumn.append(isText);
    }
    return isTextColumn;
}

QStringList DbDataExport::getColumnValueDefs(const QList<bool> &isTextColumn,
                                             const QList<QVariant> &values) {
    QStringList valueDefinitions;
    valueDefinitions.reserve(isTextColumn.size());
    for (int i = 0; i < isTextColumn.size(); ++i) {
        auto value = values.at(i).toString();
        if (isTextColumn.at(i)) {
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
    for (const auto &table : this->getDatabaseInfo().tables) {
        if (isInternalTable(table)) {
            continue;
        }
        out << "-- " << table.name << "\n";

        const auto columns = getColumnDefs(table).join(", ");
        const auto isTextColumn = getTextColumnFlags(table);
        const QueryResult streamResult = database->streamRows(
            QString("SELECT * FROM \"%1\"").arg(table.name),
            [&](const QList<QVariant> &values) {
                if (cancellationToken->isCancellationRequested())
                    return false;
                const auto valueList = getColumnValueDefs(isTextColumn, values).join(", ");
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
    for (const auto &table : this->getDatabaseInfo().tables) {
        if (isInternalTable(table) || cancellationToken->isCancellationRequested()) {
            continue;
        }

        const auto filename = outputFolder + "/" + table.name + ".csv";
        const auto file = std::make_unique<QFile>(filename);
        if (!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            return;
        }

        const auto columns = getColumnDefs(table).join(delimiter);
        const auto isTextColumn = getTextColumnFlags(table);
        QTextStream out(file.get());
        out << columns << "\n";

        const QueryResult streamResult = database->streamRows(
            QString("SELECT * FROM \"%1\"").arg(table.name),
            [&](const QList<QVariant> &values) {
                if (cancellationToken->isCancellationRequested())
                    return false;
                const auto valueList = getColumnValueDefs(isTextColumn, values).join(delimiter);
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
