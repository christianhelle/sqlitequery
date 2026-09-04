#ifndef IDATABASE_H
#define IDATABASE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <functional>

#include "queryresult.h"

class IDatabase {
public:
    virtual ~IDatabase() = default;

    virtual void setSource(const QString &filename) = 0;

    virtual bool open() = 0;

    virtual void close() = 0;

    virtual void shrink() = 0;

    [[nodiscard]] virtual QString getFilename() const = 0;

    // maxRows < 0 reads the whole result set; otherwise the read stops after
    // maxRows rows and the result is flagged as truncated.
    virtual QueryResult runStatement(const QString &sql, int maxRows = -1) = 0;

    virtual QueryResult streamRows(const QString &sql,
                                   const std::function<bool(const QList<QVariant> &)> &onRow) = 0;
};

#endif // IDATABASE_H
