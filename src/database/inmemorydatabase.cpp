#include "inmemorydatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

InMemoryDatabase::InMemoryDatabase() {
    database = QSqlDatabase::addDatabase("QSQLITE", "in_memory_connection");
    database.setDatabaseName(":memory:");
    database.setHostName("localhost");
}

void InMemoryDatabase::setSource(const QString &filename) {
    this->source = filename;
    database.setDatabaseName(filename);
}

bool InMemoryDatabase::open() {
    this->close();
    return database.open();
}

void InMemoryDatabase::close() {
    if (database.isOpen())
        database.close();
}

void InMemoryDatabase::shrink() {
    if (!database.isOpen())
        return;

    QSqlQuery query(database);
    query.exec("VACUUM");
}

QueryResult InMemoryDatabase::runStatement(const QString &sql) {
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
