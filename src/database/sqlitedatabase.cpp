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

QueryResult SqliteDatabase::runStatement(const QString &sql) {
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
        for (int i = 0; i < record.count(); ++i) {
            result.columns.append(record.fieldName(i));
        }
        while (query.next()) {
            QueryRow row;
            row.values.reserve(record.count());
            for (int i = 0; i < record.count(); ++i) {
                row.values.append(query.value(i));
            }
            result.rows.append(row);
        }
    } else {
        result.rowsAffected = query.numRowsAffected();
    }
    return result;
}
