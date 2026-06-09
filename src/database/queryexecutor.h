#ifndef QUERYEXECUTOR_H
#define QUERYEXECUTOR_H

#include <QList>
#include <QString>
#include <QStringList>

#include "idatabase.h"
#include "queryresult.h"

class QueryExecutor {
public:
    explicit QueryExecutor(IDatabase *database);

    QList<QueryResult> runScript(const QString &script, QStringList *errors = nullptr);

    QList<QueryResult> runStatements(const QStringList &statements, QStringList *errors = nullptr);

    [[nodiscard]] QueryResult previewTable(const QString &tableName, int limit = -1) const;

private:
    IDatabase *database;
};

#endif // QUERYEXECUTOR_H
