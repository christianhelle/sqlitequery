#include "sqlitedatabase.h"

#include <QSqlDatabase>
#include <QSqlQuery>

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
    // Idempotent, as it is for the in-memory adapter: re-opening used to close
    // first, which cut short any PagedResult still reading through the
    // connection. Pointing at a different file goes through setSource, which
    // closes the old one.
    if (database.isOpen())
        return true;

    return database.open();
}

void SqliteDatabase::shrink() {
    if (!database.isOpen())
        database.open();

    QSqlQuery query(database);
    query.exec("VACUUM");
}
