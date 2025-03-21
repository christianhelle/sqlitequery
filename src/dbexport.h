#ifndef DBEXPORT_H
#define DBEXPORT_H

#include "cancellation.h"
#include "database.h"
#include "databaseinfo.h"

class ExportDataProgress {
    uint64_t affectedRows = 0;
public:
    void reset() { affectedRows = 0; }
    void increment() { affectedRows++; }
    uint64_t getAffectedRows() const { return affectedRows; }
};

class DbExport {
public:
    explicit DbExport(DatabaseInfo info)
        : info(std::move(info)) {
    }

    [[nodiscard]] QString exportSchema() const;

    void exportSchemaToFile(const QString &filename) const;

    static QStringList getColumnDefinitions(const Table &table);

    [[nodiscard]] QStringList getColumnValueDefinitions(const Table &table, const QSqlQuery &query) const;

    void exportDataToFile(const Database *database,
                          const QString &filename,
                          const CancellationToken *cancellation_token,
                          ExportDataProgress *progress) const;

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
