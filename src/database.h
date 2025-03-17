#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlQuery>

class Database {
public:
    Database();

    ~Database();

    void setSource(const QString &filename);

    bool open();

    void close();

    void shrink();

    QSqlDatabase getDatabase() { return database; }
    QString getFilename() { return source; }

private:
    QSqlDatabase database;
    QString source;
};

#endif // DATABASE_H
