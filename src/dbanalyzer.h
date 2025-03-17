#ifndef DBANALYZER_H
#define DBANALYZER_H

#include <QtSql>
#include "databaseinfo.h"
#include "database.h"

class DbAnalyzer {
public:
    DbAnalyzer(Database *database);

    ~DbAnalyzer();

    bool analyze(DatabaseInfo &info);

private:
    Database *database;

    void loadTables(DatabaseInfo &info);

    void loadColumns(DatabaseInfo &info);
};

#endif // DBANALYZER_H
