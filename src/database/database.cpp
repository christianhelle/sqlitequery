#include "database.h"
#include <QSqlDatabase>

Database::Database() {
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setHostName("localhost");
}

void Database::setSource(const QString &filename) {
    this->close();
    this->source = filename;
    database.setDatabaseName(filename);
}

bool Database::open() {
    this->close();
    return database.open();
}

void Database::close() {
    if (database.isOpen())
        database.close();
}

void Database::shrink() {
    if (!database.isOpen())
        database.open();

    QSqlQuery query(database);
    query.exec("VACUUM");
}
