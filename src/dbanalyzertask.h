#ifndef DBANALYZERTASK_H
#define DBANALYZERTASK_H

#include "database.h"
#include "dbtree.h"
#include "dbanalyzer.h"

#include <QObject>
#include <QRunnable>

class DbAnalyzerTask : public QRunnable
{
public:
    DbAnalyzerTask(Database *database, DbTree *tree);
    ~DbAnalyzerTask();
    virtual void run();

private:
    Database *database;
    DbTree *tree;
};

#endif // DBANALYZERTASK_H
