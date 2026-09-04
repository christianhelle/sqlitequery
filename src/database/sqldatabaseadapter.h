#ifndef SQLDATABASEADAPTER_H
#define SQLDATABASEADAPTER_H

#include <QSqlDatabase>
#include <QString>

#include "idatabase.h"

// Shared base for the IDatabase adapters. They differ only in how their
// connection is named, created and opened; everything that reads from an open
// connection is the same for all of them and lives here.
class SqlDatabaseAdapter : public IDatabase {
public:
    void close() override;

    [[nodiscard]] QString getFilename() const override { return source; }

    QueryResult runStatement(const QString &sql) override;

    QAbstractItemModel *createResultModel(const QString &sql,
                                          QString *error = nullptr) override;

    QueryResult streamRows(const QString &sql,
                           const std::function<bool(const QList<QVariant> &)> &onRow) override;

    [[nodiscard]] QSqlDatabase getConnection() const { return database; }

protected:
    QSqlDatabase database;
    QString source;
};

#endif // SQLDATABASEADAPTER_H
