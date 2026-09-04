#include "queryexecutionpresenter.h"

QueryExecutionPresenter::QueryExecutionPresenter(QWidget *parent, QueryExecutor *executor)
    : widget(parent), executor(executor),
      presenter(std::make_unique<QueryResultPresenter>(parent)) {
}

bool QueryExecutionPresenter::execute(const QStringList &statements, QStringList *errors) {
    presenter->present(executor->runStatementsPaged(statements, errors));
    return errors == nullptr || errors->isEmpty();
}

void QueryExecutionPresenter::clearResults() {
    presenter->clear();
}

void QueryExecutionPresenter::presentToView(QTableView *view, QAbstractItemModel *model) {
    presenter->presentToView(view, model);
}
