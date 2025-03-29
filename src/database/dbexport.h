#ifndef DBEXPORT_H
#define DBEXPORT_H

#include "databaseinfo.h"

class DbExport {
public:
    explicit DbExport(DatabaseInfo info)
        : info(std::move(info)) {
    }

protected:
    DatabaseInfo getDatabaseInfo() const { return info; }
    static bool isInternalTable(const Table &table);

    const QStringList textTypes = {
        "TEXT",
        "CHARACTER",
        "VARCHAR",
        "VARYING CHARACTER",
        "NCHAR",
        "NATIVE CHARACTER",
        "NVARCHAR",
        "CLOB"
    };

private:
    DatabaseInfo info;
};


#endif //DBEXPORT_H
