#ifndef QUERYEXECUTOR_H
#define QUERYEXECUTOR_H

#include <QList>
#include <QString>
#include <QStringList>

#include "idatabase.h"
#include "queryresult.h"

class QAbstractItemModel;

class QueryExecutor {
public:
    explicit QueryExecutor(IDatabase *database);

    QList<QueryResult> runScript(const QString &script, QStringList *errors = nullptr);

    QList<QueryResult> runStatements(const QStringList &statements,
                                     QStringList *errors = nullptr);

    [[nodiscard]] QueryResult previewTable(const QString &tableName, int limit = -1) const;

    // Drops a Table. The name is delimited here rather than by the caller, so
    // a Table whose name holds a quote or a reserved word drops like any other
    // and no caller outside this module has to build the statement.
    QueryResult dropTable(const QString &tableName) const;

    // Display variants. Each returns a lazily fetched model whose rows are read
    // in pages as they are scrolled into view, so a result set of any size can
    // be browsed. The caller owns the returned models.
    QList<QAbstractItemModel *> runStatementsPaged(const QStringList &statements,
                                                   QStringList *errors = nullptr) const;

    // Splits a Script into its statements and runs them. Splitting lives here
    // rather than in a caller so that only this module knows what separates
    // one statement from the next.
    QList<QAbstractItemModel *> runScriptPaged(const QString &script,
                                               QStringList *errors = nullptr) const;

    // SQLite's own count of how many times the Schema has changed. Comparing
    // it either side of a Script says whether the Schema actually changed --
    // which a statement cannot be read off its text: a CREATE inside a
    // transaction that rolls back changes nothing, and ALTER changes plenty.
    // -1 when it could not be read.
    [[nodiscard]] int schemaVersion() const;

    [[nodiscard]] QAbstractItemModel *previewTablePaged(const QString &tableName,
                                                        QString *error = nullptr) const;

private:
    IDatabase *database;
};

#endif // QUERYEXECUTOR_H
