#ifndef DBANALYZER_H
#define DBANALYZER_H

#include "databaseinfo.h"
#include "idatabase.h"

class DbAnalyzer {
public:
    explicit DbAnalyzer(IDatabase *database);

    bool analyze(DatabaseInfo &info) const;

    void loadTables(DatabaseInfo &info) const;

    void loadColumns(DatabaseInfo &info) const;

private:
    IDatabase *database;
};

#endif // DBANALYZER_H
