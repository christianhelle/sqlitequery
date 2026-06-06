#ifndef DBQUERY_H
#define DBQUERY_H

#include "iqueryexecutor.h"
#include "iqueryresultpresenter.h"

class DbQuery {
public:
    DbQuery(QWidget *parent,
            IQueryExecutor *executor,
            IQueryResultPresenter *presenter);

    bool execute(const QStringList &queries, QStringList *errors);

    void clearResults();

private:
    QWidget *widget;
    IQueryExecutor *executor;
    IQueryResultPresenter *presenter;
};

#endif // DBQUERY_H
