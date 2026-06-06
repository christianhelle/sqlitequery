#include "databasemanager.h"

void DatabaseManager::openDatabase(IDatabase *database, const QString &filename) {
    database->setSource(filename);
    database->open();
}

void DatabaseManager::analyzeDatabase(IDatabase *database, DatabaseInfo &info) {
    if (!database->isOpen()) {
        database->open();
    }
    analyzer = DbAnalyzer(database);
    analyzer.analyze(info);
    database->close();
}

void DatabaseManager::closeDatabase(IDatabase *database) {
    database->close();
}
