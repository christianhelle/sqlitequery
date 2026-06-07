#include "inmemorydatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>

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
