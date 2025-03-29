#ifndef DBSCHEMAEXPORT_H
#define DBSCHEMAEXPORT_H

#include "dbexport.h"

class DbSchemaExport : public DbExport {
public:
    explicit DbSchemaExport(DatabaseInfo info) : DbExport(info) {
    }

    [[nodiscard]] QString exportSchema() const;

    void exportSchemaToFile(const QString &filename) const;
};

#endif // DBSCHEMAEXPORT_H
