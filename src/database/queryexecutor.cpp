#include "queryexecutor.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQueryModel>

QueryResult QueryExecutor::execute(IDatabase *database,
                                    const QStringList &queries) const {
    QueryResult result;

    if (!database->open()) {
        result.errors.append("Unable to open database");
        return result;
    }

    const QSqlDatabase db = database->getRawDatabase();
    for (const QString &sql : queries) {
        const QString trimmed = sql.trimmed().replace('\n', "", Qt::CaseInsensitive);
        if (trimmed.isEmpty())
            continue;

        QSqlQuery query(db);
        if (!query.exec(trimmed)) {
            QSqlError error;
            QString msg = (error = db.lastError()).isValid() ? error.text() : "Query execution failed";
            result.errors.append(msg);
            continue;
        }

        if (query.isSelect()) {
            QueryResult::Row row;
            const auto record = query.record();
            while (query.next()) {
                row.values.clear();
                for (int i = 0; i < record.count(); ++i) {
                    row.values.append(query.value(i));
                }
                result.rows.append(row);
            }
        }
    }

    result.success = result.errors.isEmpty();
    return result;
}
