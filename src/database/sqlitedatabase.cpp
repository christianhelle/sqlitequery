#include "sqlitedatabase.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "sqlresultreader.h"

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
    return SqlResultReader::runStatement(database, sql);
}

QAbstractItemModel *SqliteDatabase::createResultModel(const QString &sql, QString *error) {
    return SqlResultReader::createResultModel(database, sql, error);
}

QueryResult SqliteDatabase::streamRows(const QString &sql,
                                       const std::function<bool(const QList<QVariant> &)> &onRow) {
    return SqlResultReader::streamRows(database, sql, onRow);
}
