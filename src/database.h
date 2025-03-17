#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
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
    QString getFilename() { return filename; }

private:
    QSqlDatabase database;
    QString filename;
};

#endif // DATABASE_H
