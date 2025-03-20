#ifndef DBEXPORT_H
#define DBEXPORT_H

#include "databaseinfo.h"
#include "mainwindow.h"


class DbExport {
public:
    explicit DbExport(DatabaseInfo info)
        : info(std::move(info)) {
    }

    QString exportSchema() const;

    void exportSchemaToFile(const QString &filename) const;

    void exportDataToFile(const Database *database, const QString & filename) const;

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
