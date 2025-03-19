#ifndef DBANALYZER_H
#define DBANALYZER_H

#include "databaseinfo.h"
#include "database.h"

class DbAnalyzer {
public:
    explicit DbAnalyzer(Database *database);

    ~DbAnalyzer();

    bool analyze(DatabaseInfo &info) const;

    void loadTables(DatabaseInfo &info) const;

    void loadColumns(DatabaseInfo &info) const;

private:
    Database *database;
};

#endif // DBANALYZER_H
