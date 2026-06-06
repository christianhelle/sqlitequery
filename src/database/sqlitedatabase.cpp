#include "sqlitedatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>

QSqlDatabaseAdapter::QSqlDatabaseAdapter() {
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setHostName("localhost");
}

void QSqlDatabaseAdapter::setSource(const QString &filename) {
    this->close();
    this->source = filename;
    database.setDatabaseName(filename);
}

bool QSqlDatabaseAdapter::open() {
    this->close();
    bool result = database.open();
    if (result) connectionOpen = true;
    return result;
}

void QSqlDatabaseAdapter::close() {
    if (database.isOpen())
        database.close();
    connectionOpen = false;
}

void QSqlDatabaseAdapter::shrink() {
    ensureConnection();
    QSqlQuery query(database);
    query.exec("VACUUM");
}

bool QSqlDatabaseAdapter::isOpen() const {
    return database.isOpen();
}

QString QSqlDatabaseAdapter::getFilename() const {
    return source;
}

bool QSqlDatabaseAdapter::execute(const QString &sql) {
    ensureConnection();
    QSqlQuery query(database);
    return query.exec(sql);
}

QList<QString> QSqlDatabaseAdapter::getTableNames() const {
    QList<QString> tables;
    ensureConnection();
    QSqlQuery query(database);
    if (query.exec("SELECT name FROM sqlite_master WHERE type='table'")) {
        while (query.next()) {
            tables.append(query.value("name").toString());
        }
    }
    return tables;
}

bool QSqlDatabaseAdapter::hasOpenConnection() const {
    return database.isOpen();
}

void QSqlDatabaseAdapter::ensureConnection() const {
    if (!database.isOpen()) {
        connectionWasOpen = false;
        const_cast<QSqlDatabaseAdapter*>(this)->database.open();
        connectionOpen = true;
    } else {
        connectionWasOpen = true;
    }
}
