#ifndef DBANALYZER_H
#define DBANALYZER_H

#include "databaseinfo.h"
#include "idatabase.h"

class DbAnalyzer {
public:
    explicit DbAnalyzer(IDatabase *database);

    // False when the Schema could not be read, so a caller does not build a
    // Tree out of an empty snapshot. Reads through the seam and leaves the
    // connection exactly as it found it: opening and closing a Database is the
    // job of whoever owns it, not of a module that only reads.
    bool analyze(DatabaseInfo &info) const;

    bool loadTables(DatabaseInfo &info) const;

    void loadColumns(DatabaseInfo &info) const;

private:
    IDatabase *database;
};

#endif // DBANALYZER_H
