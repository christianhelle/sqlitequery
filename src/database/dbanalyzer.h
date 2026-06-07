#ifndef DBANALYZER_H
#define DBANALYZER_H

#include "databaseinfo.h"
#include "sqlitedatabase.h"

class DbAnalyzer {
public:
    explicit DbAnalyzer(SqliteDatabase *database);

    bool analyze(DatabaseInfo &info) const;

    void loadTables(DatabaseInfo &info) const;

    void loadColumns(DatabaseInfo &info) const;

private:
    SqliteDatabase *database;
};

#endif // DBANALYZER_H
