#include "sqlitedatabase.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

SqliteDatabase::SqliteDatabase() {
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setHostName("localhost");
}

void SqliteDatabase::setSource(const QString &filename) {
    this->close();
    this->source = filename;
    database.setDatabaseName(filename);
}

bool SqliteDatabase::open() {
    this->close();
    return database.open();
}

void SqliteDatabase::close() {
    if (database.isOpen())
        database.close();
}

void SqliteDatabase::shrink() {
    if (!database.isOpen())
        database.open();

    QSqlQuery query(database);
    query.exec("VACUUM");
}

QueryResult SqliteDatabase::runStatement(const QString &sql, const int maxRows) {
    QueryResult result;
    if (!database.isOpen()) {
        result.ok = false;
        result.error = "Database is not open";
        return result;
    }

    QSqlQuery query(database);
    if (!query.exec(sql)) {
        result.ok = false;
        result.error = query.lastError().text();
        return result;
    }

    result.isSelect = query.isSelect();
    if (result.isSelect) {
        const QSqlRecord record = query.record();
        const int columnCount = record.count();
        result.columns.reserve(columnCount);
        for (int i = 0; i < columnCount; ++i) {
            result.columns.append(record.fieldName(i));
        }
        while (query.next()) {
            if (maxRows >= 0 && result.rows.size() >= maxRows) {
                result.truncated = true;
                break;
            }
            QueryRow row;
            row.values.reserve(columnCount);
            for (int i = 0; i < columnCount; ++i) {
                row.values.append(query.value(i));
            }
            result.rows.append(std::move(row));
        }
    } else {
        result.rowsAffected = query.numRowsAffected();
    }
    return result;
}

QueryResult SqliteDatabase::streamRows(const QString &sql,
                                       const std::function<bool(const QList<QVariant> &)> &onRow) {
    QueryResult result;
    if (!database.isOpen()) {
        result.ok = false;
        result.error = "Database is not open";
        return result;
    }

    QSqlQuery query(database);
    query.setForwardOnly(true);
    if (!query.exec(sql)) {
        result.ok = false;
        result.error = query.lastError().text();
        return result;
    }

    result.isSelect = query.isSelect();
    const QSqlRecord record = query.record();
    const int columnCount = record.count();
    result.columns.reserve(columnCount);
    for (int i = 0; i < columnCount; ++i) {
        result.columns.append(record.fieldName(i));
    }

    QList<QVariant> values;
    values.reserve(columnCount);
    while (query.next()) {
        values.clear();
        for (int i = 0; i < columnCount; ++i) {
            values.append(query.value(i));
        }
        if (!onRow(values))
            break;
    }
    return result;
}
