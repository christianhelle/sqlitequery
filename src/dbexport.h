#ifndef DBEXPORT_H
#define DBEXPORT_H

#include "databaseinfo.h"


class DbExport {
public:
    explicit DbExport(DatabaseInfo info)
        : info(std::move(info)) {
    }

    QString exportSchema() const;
    void exportSchemaToFile(const QString &filename) const;

private:
    DatabaseInfo info;

    static bool isInternalTable(const Table &table);
};


#endif //DBEXPORT_H
