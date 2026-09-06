#ifndef QUERYEXECUTIONPRESENTER_H
#define QUERYEXECUTIONPRESENTER_H

#include <QList>
#include <QStringList>
#include <QWidget>
#include <memory>

#include "../database/queryexecutor.h"
#include "../database/queryresult.h"
#include "queryresultpresenter.h"

// What running a Script did. Every field is a fact about the run, not a
// decision about what to show -- the window decides that.
struct ScriptOutcome {
    QStringList errors;
    // Wall clock time the run took, in milliseconds.
    qint64 elapsedMs = 0;
    // Whether the Schema actually changed, according to SQLite rather than to
    // the text of the statements.
    bool schemaChanged = false;

    [[nodiscard]] bool ok() const { return errors.isEmpty(); }
};

class QueryExecutionPresenter {
public:
    explicit QueryExecutionPresenter(QWidget *parent, QueryExecutor *executor);

    // Runs a Script and renders whatever it produced.
    ScriptOutcome run(const QString &script);

    void clearResults();

    void presentToView(QTableView *view, QAbstractItemModel *model);

    // The views the last run rendered into.
    [[nodiscard]] const QList<QTableView *> &resultViews() const;

private:
    QueryExecutor *executor;
    std::unique_ptr<QueryResultPresenter> presenter;
};

#endif // QUERYEXECUTIONPRESENTER_H
