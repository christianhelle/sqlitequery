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

    [[nodiscard]] QSqlDatabase getDatabase() const { return database; }
    [[nodiscard]] QString getFilename() const { return source; }

private:
    QSqlDatabase database;
    QString source;
};

#endif // DATABASE_H
