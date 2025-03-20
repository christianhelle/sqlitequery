#ifndef DBEXPORT_H
#define DBEXPORT_H

#include "databaseinfo.h"
#include "mainwindow.h"


class DbExport {
public:
    explicit DbExport(DatabaseInfo info)
        : info(std::move(info)) {
    }

    [[nodiscard]] QString exportSchema() const;

    void exportSchemaToFile(const QString &filename) const;

    static QStringList getColumnDefinitions(const Table &table);

    [[nodiscard]] QStringList getColumnValueDefinitions(const Table &table, const QSqlQuery &query) const;

    void exportDataToFile(const Database *database, const QString &filename, const bool *cancel) const;

private:
    DatabaseInfo info;

    static bool isInternalTable(const Table &table);

    QStringList textTypes = {
        "TEXT",
        "CHARACTER",
        "VARCHAR",
        "VARYING CHARACTER",
        "NCHAR",
        "NATIVE CHARACTER",
        "NVARCHAR",
        "CLOB"
    };
};


#endif //DBEXPORT_H
