#include "dbquery.h"
#include "../gui/queryresultpresenter.h"

#include <QMessageBox>

DbQuery::DbQuery(QWidget *widget, IDatabase *database)
    : widget(widget),
      database(database) {
    this->executor = std::make_unique<QueryExecutor>(database);
    this->presenter = std::make_unique<QueryResultPresenter>(widget);
}

DbQuery::~DbQuery() = default;

void DbQuery::clearResults() {
    this->presenter->clear();
}

bool DbQuery::execute(const QStringList &queryList, QStringList *errors) {
    this->presenter->clear();

    if (!this->database->open()) {
        const QString msg = "Unable to open database";
        errors->append(msg);
        return false;
    }

    const QList<QueryResult> results = this->executor->runStatements(queryList, errors);
    this->presenter->present(results);

    if (errors != nullptr && !errors->empty()) {
        const QString msg = errors->join("\r\n");
        QMessageBox::information(nullptr, "Error", msg, QMessageBox::Ok);
        return false;
    }

    return true;
}
