#include "inmemorydatabase.h"

#include <QSqlDatabase>
#include <QSqlQuery>

InMemoryDatabase::InMemoryDatabase() {
    database = QSqlDatabase::addDatabase("QSQLITE", "in_memory_connection");
    database.setDatabaseName(":memory:");
    database.setHostName("localhost");
}

// Deliberately does not close first, unlike the production adapter: for an
// in-memory database the data *is* the connection, so closing would discard
// it. Nothing switches source on this adapter.
void InMemoryDatabase::setSource(const QString &filename) {
    this->source = filename;
    database.setDatabaseName(filename);
}

bool InMemoryDatabase::open() {
    if (database.isOpen())
        return true;
    return database.open();
}

void InMemoryDatabase::shrink() {
    if (!database.isOpen())
        return;

    QSqlQuery query(database);
    query.exec("VACUUM");
}
