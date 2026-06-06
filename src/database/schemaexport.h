#ifndef SCHEMAEXPORT_H
#define SCHEMAEXPORT_H

#include "ischemaexport.h"
#include "databaseinfo.h"

class SchemaExport : public ISchemaScript {
public:
    explicit SchemaExport(DatabaseInfo info);

    [[nodiscard]] QString exportSchema() const override;

private:
    DatabaseInfo info;
    static bool isInternalTable(const Table &table);
};

#endif // SCHEMAEXPORT_H
