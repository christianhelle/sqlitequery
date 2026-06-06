#include "databasemanager.h"
#include "../database/dbanalyzer.h"

void DatabaseManager::openDatabase(IDatabase *database, const QString &filename) {
    database->setSource(filename);
    database->open();
}

void DatabaseManager::analyzeDatabase(IDatabase *database, DatabaseInfo &info) {
    if (!database->isOpen()) {
        database->open();
    }
    DbAnalyzer analyzer(database);
    analyzer.analyze(info);
    database->close();
}

void DatabaseManager::closeDatabase(IDatabase *database) {
    database->close();
}
