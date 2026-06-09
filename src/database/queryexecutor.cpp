#include "queryexecutor.h"

QueryExecutor::QueryExecutor(IDatabase *database)
    : database(database) {
}

QList<QueryResult> QueryExecutor::runScript(const QString &script, QStringList *errors) {
    const QStringList raw = script.split(";", Qt::SkipEmptyParts);
    return runStatements(raw, errors);
}

QList<QueryResult> QueryExecutor::runStatements(const QStringList &statements, QStringList *errors) {
    QList<QueryResult> results;
    for (const auto &raw: statements) {
        const QString sql = raw.trimmed().replace('\n', "");
        if (sql.isEmpty())
            continue;

        QueryResult result = database->runStatement(sql);
        if (!result.ok) {
            if (errors != nullptr)
                errors->append(result.error.isEmpty() ? QString("Query failed") : result.error);
        }
        results.append(result);
    }
    return results;
}

QueryResult QueryExecutor::previewTable(const QString &tableName, int limit) const {
    QString sql = QString("SELECT * FROM \"%1\"").arg(tableName);
    if (limit > 0) {
        sql += QString(" LIMIT %1").arg(limit);
    }
    return database->runStatement(sql);
}
