#include "dataexport.h"
#include <QFile>

DataExport::DataExport(DatabaseInfo info)
    : info(std::move(info)) {
}

QStringList DataExport::getColumnDefs(const Table &table) {
    QStringList columnDefinitions;
    for (const auto &column: table.columns) {
        columnDefinitions.append(column.name);
    }
    return columnDefinitions;
}

QStringList DataExport::getColumnValueDefs(const Table &table,
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

QStringList DataExport::getTextTypes() const {
    return {
        "TEXT",
        "CHARACTER",
        "VARCHAR",
        "VARYING CHARACTER",
        "NCHAR",
        "NATIVE CHARACTER",
        "NVARCHAR",
        "CLOB"
    };
}

bool DataExport::isInternalTable(const Table &table) {
    if (table.name == "sqlite_sequence" || table.name == "sqlite_stat1") {
        return true;
    }
    return false;
}

void DataExport::exportToSqlFile(IDatabase *database,
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
    for (const auto &table: info.tables) {
        if (isInternalTable(table)) {
            continue;
        }
        out << "-- " << table.name << "\n";

        QSqlQuery query(database->getRawDatabase());
        query.setForwardOnly(true);
        if (!query.exec(QString("SELECT * FROM \"%1\"").arg(table.name))) {
            continue;
        }
        const auto columns = getColumnDefs(table).join(", ");
        while (query.next() && !cancellationToken->isCancellationRequested()) {
            const auto values = getColumnValueDefs(table, query).join(", ");
            out << "INSERT INTO \"" << table.name << "\"(" << columns << ") ";
            out << "VALUES (" << values << ");\n";
            progress->increment();
        }
        query.finish();
        out << "\n";
    }
    file->close();
    progress->setCompleted();
}

void DataExport::exportToCsvFile(IDatabase *database,
                                  const QString &outputFolder,
                                  const QString &delimiter,
                                  const CancellationToken *cancellationToken,
                                  ExportDataProgress *progress) const {
    progress->reset();
    for (const auto &table: info.tables) {
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

        QSqlQuery query(database->getRawDatabase());
        query.setForwardOnly(true);
        if (!query.exec(QString("SELECT * FROM \"%1\"").arg(table.name))) {
            file->close();
            continue;
        }

        while (query.next() && !cancellationToken->isCancellationRequested()) {
            const auto values = getColumnValueDefs(table, query).join(delimiter);
            out << values << "\n";
            progress->increment();
        }
        query.finish();

        file->close();
    }

    progress->setCompleted();
}
