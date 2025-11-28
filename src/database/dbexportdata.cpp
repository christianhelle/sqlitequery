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
                                             const QSqlQuery &query) const {
    QStringList valueDefinitions;
    for (const auto &column: table.columns) {
        auto value = query.value(column.name).toString();
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

void DbDataExport::exportDataToSqlFile(const Database *database,
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

        QSqlQuery query(database->getDatabase());
        query.setForwardOnly(true);
        query.exec(QString("SELECT * FROM %1").arg(table.name));
        const auto columns = getColumnDefs(table).join(", ");
        while (query.next() && !cancellationToken->isCancellationRequested()) {
            const auto values = getColumnValueDefs(table, query).join(", ");
            out << "INSERT INTO " << table.name << "(" << columns << ") ";
            out << "VALUES (" << values << ");\n";
            progress->increment();
        }
        query.finish();
        out << "\n";
    }
    file->close();
    progress->setCompleted();
}

void DbDataExport::exportDataToCsvFile(const Database *database,
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
        out << columns;

        QSqlQuery query(database->getDatabase());
        query.setForwardOnly(true);
        query.exec(QString("SELECT * FROM %1").arg(table.name));

        while (query.next() && !cancellationToken->isCancellationRequested()) {
            const auto values = getColumnValueDefs(table, query).join(delimiter);
            out << values << "\n";
            progress->increment();
        }
        query.finish();

        out << "\n";
        file->close();
    }

    progress->setCompleted();
}
