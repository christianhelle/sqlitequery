#include "database.h"
#include <QSqlDatabase>

Database::Database() {
    qDebug("Database::Database()");

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setHostName("localhost");
}

void Database::setSource(const QString &filename) {
    qDebug("Database::setSource(QString)");

    this->close();
    this->source = filename;
    database.setDatabaseName(filename);
}

bool Database::open() {
    qDebug("Database::open()");

    this->close();
    return database.open();
}

void Database::close() {
    qDebug("Database::close()");

    if (database.isOpen())
        database.close();
}

void Database::shrink() {
    qDebug("Database::shrink()");

    if (!database.isOpen())
        database.open();

    QSqlQuery query(database);
    query.exec("VACUUM");
}
