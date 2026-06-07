#ifndef DBQUERY_H
#define DBQUERY_H

#include <QString>
#include <QStringList>
#include <memory>

#include "../database/queryexecutor.h"

class QueryResultPresenter;

class DbQuery {
public:
    DbQuery(QWidget *, IDatabase *);
    ~DbQuery();

    bool execute(const QStringList &, QStringList *);

    void clearResults();

private:
    QWidget *widget;
    IDatabase *database;
    std::unique_ptr<QueryExecutor> executor;
    std::unique_ptr<QueryResultPresenter> presenter;
};

#endif // DBQUERY_H
