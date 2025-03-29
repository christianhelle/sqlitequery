#ifndef DBDATAEXPORT_H
#define DBDATAEXPORT_H

#include "dbexport.h"
#include "database.h"
#include "../threading/cancellation.h"

#include <cstdint>
#include <utility>

class ExportDataProgress {
    uint64_t affectedRows = 0;
    bool isComplete = false;

public:
    void reset() {
        affectedRows = 0;
        isComplete = false;
    }

    void increment() {
        affectedRows++;
        isComplete = false;
    }

    [[nodiscard]] uint64_t getAffectedRows() const { return affectedRows; }
    [[nodiscard]] bool isCompleted() const { return isComplete; }
    void setCompleted() { isComplete = true; }
};

class DbDataExport : public DbExport {
public:
    explicit DbDataExport(DatabaseInfo info) : DbExport(std::move(info)) {
    }

    void exportDataToFile(const Database *database,
                          const QString &filename,
                          const CancellationToken *cancellationToken,
                          ExportDataProgress *progress) const;

private:
    static QStringList getColumnDefs(const Table &table);

    [[nodiscard]] QStringList getColumnValueDefs(const Table &table,
                                                 const QSqlQuery &query) const;
};

#endif // DBDATAEXPORT_H
