#ifndef INMEMORYDATABASE_H
#define INMEMORYDATABASE_H

#include <QSqlDatabase>
#include <QString>

#include "idatabase.h"

class InMemoryDatabase : public IDatabase {
public:
    InMemoryDatabase();

    void setSource(const QString &filename) override;

    bool open() override;

    void close() override;

    void shrink() override;

    [[nodiscard]] QString getFilename() const override { return source; }

    [[nodiscard]] QSqlDatabase getConnection() const { return database; }

private:
    QSqlDatabase database;
    QString source;
};

#endif // INMEMORYDATABASE_H
