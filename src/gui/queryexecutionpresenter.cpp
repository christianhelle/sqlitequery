#include "queryexecutionpresenter.h"

QueryExecutionPresenter::QueryExecutionPresenter(QWidget *parent, QueryExecutor *executor)
    : widget(parent), executor(executor),
      presenter(std::make_unique<QueryResultPresenter>(parent)) {
}

bool QueryExecutionPresenter::execute(const QStringList &statements, QStringList *errors) {
    const QList<QueryResult> results = executor->runStatements(statements, errors);
    presenter->present(results);
    return errors == nullptr || errors->isEmpty();
}

void QueryExecutionPresenter::clearResults() {
    presenter->clear();
}

void QueryExecutionPresenter::presentToView(QTableView *view, const QueryResult &result) {
    presenter->presentToView(view, result);
}
