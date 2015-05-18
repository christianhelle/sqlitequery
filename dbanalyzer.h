#ifndef DBANALYZER_H
#define DBANALYZER_H

#include <QtSql>
#include "databaseinfo.h"

class DatabaseAnalyzer
{
public:
    DatabaseAnalyzer();
    ~DatabaseAnalyzer();

    bool open(QString filename);
    void close();
    void shrink();
    bool analyze(DatabaseInfo &info);

    void loadTables(DatabaseInfo &info);
    void loadColumns(DatabaseInfo &info);

    QSqlDatabase getDatabase() { return database; }
    QString getFilename() { return filename; }

private:
    QSqlDatabase database;
    QString filename;
};

#endif // DBANALYZER_H
