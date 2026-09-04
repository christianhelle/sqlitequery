#ifndef QUERYEXECUTIONPRESENTER_H
#define QUERYEXECUTIONPRESENTER_H

#include <QList>
#include <QStringList>
#include <QWidget>
#include <memory>

#include "../database/queryexecutor.h"
#include "../database/queryresult.h"
#include "queryresultpresenter.h"

class QueryExecutionPresenter {
public:
    explicit QueryExecutionPresenter(QWidget *parent, QueryExecutor *executor);

    bool execute(const QStringList &statements, QStringList *errors = nullptr);

    void clearResults();

    void presentToView(QTableView *view, QAbstractItemModel *model);

private:
    QWidget *widget;
    QueryExecutor *executor;
    std::unique_ptr<QueryResultPresenter> presenter;
};

#endif // QUERYEXECUTIONPRESENTER_H
