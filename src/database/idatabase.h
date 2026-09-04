#ifndef IDATABASE_H
#define IDATABASE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <functional>

#include "queryresult.h"

class QAbstractItemModel;

class IDatabase {
public:
    virtual ~IDatabase() = default;

    virtual void setSource(const QString &filename) = 0;

    virtual bool open() = 0;

    virtual void close() = 0;

    virtual void shrink() = 0;

    [[nodiscard]] virtual QString getFilename() const = 0;

    virtual QueryResult runStatement(const QString &sql) = 0;

    // A lazily fetched, pageable model over the statement's result set. Rows
    // are read in pages as they are scrolled into view, so a result set of any
    // size can be browsed. The caller owns the returned model.
    // Returns nullptr when the statement produces no result set (an INSERT,
    // say) or when it failed -- `error` then holds the reason, and is cleared
    // otherwise. A SELECT matching no rows still returns a model, so that its
    // columns can be displayed.
    virtual QAbstractItemModel *createResultModel(const QString &sql,
                                                  QString *error = nullptr) = 0;

    virtual QueryResult streamRows(const QString &sql,
                                   const std::function<bool(const QList<QVariant> &)> &onRow) = 0;
};

#endif // IDATABASE_H
