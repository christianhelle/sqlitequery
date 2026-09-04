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

    // maxRows < 0 reads every row; otherwise each statement stops after
    // maxRows rows and its result is flagged as truncated.
    QList<QueryResult> runStatements(const QStringList &statements,
                                     QStringList *errors = nullptr,
                                     int maxRows = -1);

    [[nodiscard]] QueryResult previewTable(const QString &tableName, int limit = -1) const;

    // Display variants. Each returns a lazily fetched model whose rows are read
    // in pages as they are scrolled into view, so a result set of any size can
    // be browsed. The caller owns the returned models.
    QList<QAbstractItemModel *> runStatementsPaged(const QStringList &statements,
                                                   QStringList *errors = nullptr) const;

    [[nodiscard]] QAbstractItemModel *previewTablePaged(const QString &tableName,
                                                        QString *error = nullptr) const;

private:
    IDatabase *database;
};

#endif // QUERYEXECUTOR_H
