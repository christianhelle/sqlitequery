#ifndef SQLRESULTREADER_H
#define SQLRESULTREADER_H

#include <QList>
#include <QString>
#include <QVariant>
#include <functional>

#include "queryresult.h"

class QAbstractItemModel;
class QSqlDatabase;

// Shared implementation behind the IDatabase adapters. The adapters differ only
// in which connection they hold, so the reading itself lives here instead of
// being copied into each of them.
namespace SqlResultReader {
    QueryResult runStatement(const QSqlDatabase &database, const QString &sql);

    QAbstractItemModel *createResultModel(const QSqlDatabase &database,
                                          const QString &sql,
                                          QString *error);

    QueryResult streamRows(const QSqlDatabase &database,
                           const QString &sql,
                           const std::function<bool(const QList<QVariant> &)> &onRow);
}

#endif // SQLRESULTREADER_H
