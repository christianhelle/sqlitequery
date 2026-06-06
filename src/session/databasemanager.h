#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "idatabasemanager.h"
#include "../database/databaseinfo.h"

class DatabaseManager : public IDatabaseManager {
public:
    void openDatabase(IDatabase *database, const QString &filename) override;
    void analyzeDatabase(IDatabase *database, DatabaseInfo &info) override;
    void closeDatabase(IDatabase *database) override;
};

#endif // DATABASEMANAGER_H
