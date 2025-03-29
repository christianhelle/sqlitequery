#ifndef DBEXPORT_H
#define DBEXPORT_H

#include "databaseinfo.h"

class DbExport {
public:
    explicit DbExport(DatabaseInfo info)
        : info(std::move(info)) {
    }

protected:
    [[nodiscard]] DatabaseInfo getDatabaseInfo() const { return info; }
    [[nodiscard]] QStringList getTextTypes() const { return textTypes; }
    static bool isInternalTable(const Table &table);

private:
    DatabaseInfo info;
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
};


#endif //DBEXPORT_H
