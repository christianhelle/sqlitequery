#ifndef INMEMORYDATABASE_H
#define INMEMORYDATABASE_H

#include <QString>

#include "sqldatabaseadapter.h"

// Test adapter: a SQLite database held in memory.
class InMemoryDatabase final : public SqlDatabaseAdapter {
public:
    InMemoryDatabase();

    void setSource(const QString &filename) override;

    bool open() override;

    void shrink() override;
};

#endif // INMEMORYDATABASE_H
