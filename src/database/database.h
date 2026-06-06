#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlQuery>
#include "idatabase.h"

class Database : public IDatabase {
public:
    Database();

    void setSource(const QString &filename) override;

    bool open() override;

    void close() override;

    void shrink() override;

    bool isOpen() const override;

    [[nodiscard]] QString getFilename() const override;

    QSqlDatabase getDatabase() const { return database; }

private:
    QSqlDatabase database;
    QString source;
};

#endif // DATABASE_H
