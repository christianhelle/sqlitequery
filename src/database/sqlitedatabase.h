#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <QSqlDatabase>
#include <QString>

#include "idatabase.h"

class SqliteDatabase : public IDatabase {
public:
    SqliteDatabase();

    void setSource(const QString &filename) override;

    bool open() override;

    void close() override;

    void shrink() override;

    [[nodiscard]] QString getFilename() const override { return source; }

    QueryResult runStatement(const QString &sql, int maxRows = -1) override;

    QueryResult streamRows(const QString &sql,
                           const std::function<bool(const QList<QVariant> &)> &onRow) override;

    [[nodiscard]] QSqlDatabase getConnection() const { return database; }

private:
    QSqlDatabase database;
    QString source;
};

#endif // SQLITEDATABASE_H
