#include "dbquery.h"

DbQuery::DbQuery(QWidget *parent,
                 IQueryExecutor *executor,
                 IQueryResultPresenter *presenter)
    : widget(parent),
      executor(executor),
      presenter(presenter) {
}

void DbQuery::clearResults() {
    presenter->clear();
}

bool DbQuery::execute(const QStringList &queries, QStringList *errors) {
    this->clearResults();

    const QueryResult result = executor->execute(nullptr, queries);

    presenter->display(result, widget);

    if (errors) {
        *errors = result.errors;
    }

    return result.success;
}
