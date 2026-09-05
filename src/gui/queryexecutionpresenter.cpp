#include "queryexecutionpresenter.h"

#include <QElapsedTimer>

QueryExecutionPresenter::QueryExecutionPresenter(QWidget *parent, QueryExecutor *executor)
    : executor(executor),
      presenter(std::make_unique<QueryResultPresenter>(parent)) {
}

ScriptOutcome QueryExecutionPresenter::run(const QString &script) {
    ScriptOutcome outcome;

    // Read either side of the run rather than reading the statements: a CREATE
    // inside a transaction that rolls back changes nothing, and an ALTER
    // changes the Schema without saying so in a word this could match on.
    const int before = executor->schemaVersion();

    QElapsedTimer time;
    time.start();
    presenter->present(executor->runScriptPaged(script, &outcome.errors));
    outcome.elapsedMs = time.elapsed();

    const int after = executor->schemaVersion();
    outcome.schemaChanged = before >= 0 && after >= 0 && before != after;

    return outcome;
}

void QueryExecutionPresenter::clearResults() {
    presenter->clear();
}

void QueryExecutionPresenter::presentToView(QTableView *view, QAbstractItemModel *model) {
    presenter->presentToView(view, model);
}
