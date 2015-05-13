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
    bool analyze(DatabaseInfo &info);

private:
    QSqlDatabase database;
    QString filename;
};

#endif // DBANALYZER_H
