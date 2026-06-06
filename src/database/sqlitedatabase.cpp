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
    return database.open();
}

void QSqlDatabaseAdapter::close() {
    if (database.isOpen())
        database.close();
}

void QSqlDatabaseAdapter::shrink() {
    if (ensureOpen()) {
        QSqlQuery query(database);
        query.exec("VACUUM");
    }
}

bool QSqlDatabaseAdapter::isOpen() const {
    return database.isOpen();
}

QString QSqlDatabaseAdapter::getFilename() const {
    return source;
}

bool QSqlDatabaseAdapter::execute(const QString &sql) const {
    if (!ensureOpen())
        return false;

    QSqlQuery query(database);
    return query.exec(sql);
}

QList<QString> QSqlDatabaseAdapter::getTableNames() const {
    QList<QString> tables;
    if (!ensureOpen())
        return tables;

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

bool QSqlDatabaseAdapter::ensureOpen() const {
    if (!database.isOpen()) {
        return database.open();
    }
    return true;
}
