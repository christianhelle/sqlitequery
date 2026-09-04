#include "inmemorydatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include "sqlresultreader.h"
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
    if (database.isOpen())
        return true;
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
    return SqlResultReader::runStatement(database, sql);
}

QAbstractItemModel *InMemoryDatabase::createResultModel(const QString &sql, QString *error) {
    return SqlResultReader::createResultModel(database, sql, error);
}

QueryResult InMemoryDatabase::streamRows(const QString &sql,
                                         const std::function<bool(const QList<QVariant> &)> &onRow) {
    return SqlResultReader::streamRows(database, sql, onRow);
}
