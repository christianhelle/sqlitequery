#ifndef IDATABASEMANAGER_H
#define IDATABASEMANAGER_H

#include "idatabase.h"
#include "databaseinfo.h"

class IDatabaseManager {
public:
    virtual ~IDatabaseManager() = default;

    virtual void openDatabase(IDatabase *database, const QString &filename) = 0;
    virtual void analyzeDatabase(IDatabase *database, DatabaseInfo &info) = 0;
    virtual void closeDatabase(IDatabase *database) = 0;
};

#endif // IDATABASEMANAGER_H
