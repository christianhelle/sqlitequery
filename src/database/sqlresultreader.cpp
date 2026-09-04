#include "sqlresultreader.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <memory>

#include "pagedresultmodel.h"

QueryResult SqlResultReader::runStatement(const QSqlDatabase &database, const QString &sql) {
    QueryResult result;
    if (!database.isOpen()) {
        result.ok = false;
        result.error = "Database is not open";
        return result;
    }

    QSqlQuery query(database);
    if (!query.exec(sql)) {
        result.ok = false;
        result.error = query.lastError().text();
        return result;
    }

    result.isSelect = query.isSelect();
    if (result.isSelect) {
        const QSqlRecord record = query.record();
        const int columnCount = record.count();
        result.columns.reserve(columnCount);
        for (int i = 0; i < columnCount; ++i) {
            result.columns.append(record.fieldName(i));
        }
        while (query.next()) {
            QueryRow row;
            row.values.reserve(columnCount);
            for (int i = 0; i < columnCount; ++i) {
                row.values.append(query.value(i));
            }
            result.rows.append(std::move(row));
        }
    } else {
        result.rowsAffected = query.numRowsAffected();
    }
    return result;
}

QAbstractItemModel *SqlResultReader::createResultModel(const QSqlDatabase &database,
                                                      const QString &sql,
                                                      QString *error) {
    if (!database.isOpen()) {
        if (error != nullptr)
            *error = "Database is not open";
        return nullptr;
    }

    auto model = std::make_unique<PagedResultModel>(database, sql);
    if (!model->errorText().isEmpty()) {
        if (error != nullptr)
            *error = model->errorText();
        return nullptr;
    }

    // The statement ran but returns no rows, so there is nothing to display.
    if (model->columnCount() == 0)
        return nullptr;

    return model.release();
}

QueryResult SqlResultReader::streamRows(const QSqlDatabase &database,
                                        const QString &sql,
                                        const std::function<bool(const QList<QVariant> &)> &onRow) {
    QueryResult result;
    if (!database.isOpen()) {
        result.ok = false;
        result.error = "Database is not open";
        return result;
    }

    QSqlQuery query(database);
    query.setForwardOnly(true);
    if (!query.exec(sql)) {
        result.ok = false;
        result.error = query.lastError().text();
        return result;
    }

    result.isSelect = query.isSelect();
    const QSqlRecord record = query.record();
    const int columnCount = record.count();
    result.columns.reserve(columnCount);
    for (int i = 0; i < columnCount; ++i) {
        result.columns.append(record.fieldName(i));
    }

    QList<QVariant> values;
    values.reserve(columnCount);
    while (query.next()) {
        values.clear();
        for (int i = 0; i < columnCount; ++i) {
            values.append(query.value(i));
        }
        if (!onRow(values))
            break;
    }
    return result;
}
