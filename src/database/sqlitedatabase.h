#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include "idatabase.h"

class QSqlDatabaseAdapter : public IDatabase {
public:
    QSqlDatabaseAdapter();

    void setSource(const QString &filename) override;
    bool open() override;
    void close() override;
    void shrink() override;
    bool isOpen() const override;
    [[nodiscard]] QString getFilename() const override;

    QSqlDatabase getRawDatabase() const { return database; }
    bool execute(const QString &sql) const;
    QList<QString> getTableNames() const;
    bool hasOpenConnection() const;

private:
    QSqlDatabase database;
    QString source;
    bool ensureOpen() const;
};

#endif // SQLITEDATABASE_H
