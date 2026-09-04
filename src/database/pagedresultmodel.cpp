#include "pagedresultmodel.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <utility>

PagedResultModel::PagedResultModel(const QSqlDatabase &database, QString sql, QObject *parent)
    : QSqlQueryModel(parent),
      database(database),
      statement(std::move(sql)) {
    run(statement);
}

bool PagedResultModel::run(const QString &sql) {
    QSqlQuery query(this->database);
    if (!query.exec(sql)) {
        this->error = query.lastError().text();
        return false;
    }

    // QSqlQueryModel reads the first page here and the rest on demand.
    setQuery(std::move(query));
    if (lastError().isValid()) {
        this->error = lastError().text();
        return false;
    }

    this->error.clear();
    return true;
}

void PagedResultModel::sort(const int column, const Qt::SortOrder order) {
    if (column < 0 || column >= columnCount())
        return;

    QString columnName = record().fieldName(column);
    if (columnName.isEmpty())
        return;

    // A quoted identifier escapes a double quote by doubling it.
    columnName.replace('"', "\"\"");

    const QString sorted = QString("SELECT * FROM (%1) ORDER BY \"%2\" %3")
            .arg(statement,
                 columnName,
                 order == Qt::AscendingOrder ? "ASC" : "DESC");

    // Not every statement can be wrapped in a subquery. run() leaves the model
    // untouched when it fails, so there is nothing to restore -- and re-running
    // the original would execute a statement with side effects a second time.
    run(sorted);
}
