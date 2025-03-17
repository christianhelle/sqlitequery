#ifndef DBANALYZER_H
#define DBANALYZER_H

#include <QtSql>
#include "databaseinfo.h"
#include "database.h"

class DbAnalyzer {
public:
    explicit DbAnalyzer(Database *database);

    ~DbAnalyzer();

    bool analyze(DatabaseInfo &info) const;

private:
    Database *database;

    void loadTables(DatabaseInfo &info) const;

    void loadColumns(DatabaseInfo &info) const;
};

#endif // DBANALYZER_H
