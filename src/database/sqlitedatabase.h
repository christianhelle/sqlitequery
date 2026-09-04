#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <QString>

#include "sqldatabaseadapter.h"

// Production adapter: a SQLite database file on disk.
class SqliteDatabase final : public SqlDatabaseAdapter {
public:
    SqliteDatabase();

    void setSource(const QString &filename) override;

    bool open() override;

    void shrink() override;
};

#endif // SQLITEDATABASE_H
