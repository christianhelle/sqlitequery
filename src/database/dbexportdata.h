#ifndef DBDATAEXPORT_H
#define DBDATAEXPORT_H

#include "dbexport.h"
#include "idatabase.h"
#include "../threading/cancellation.h"

#include <utility>

#include "progress.h"

class DbDataExport : public DbExport {
public:
    explicit DbDataExport(DatabaseInfo info) :
        DbExport(std::move(info)) {
    }

    void exportDataToSqlFile(IDatabase *database,
                              const QString &filename,
                              const CancellationToken *cancellationToken,
                              ExportDataProgress *progress) const;

    void exportDataToCsvFile(IDatabase *database,
                              const QString &outputFolder,
                              const QString &delimiter,
                              const CancellationToken *cancellationToken,
                              ExportDataProgress *progress) const;

private:
    static QStringList getColumnDefs(const Table &table);

    [[nodiscard]] QStringList getColumnValueDefs(const Table &table,
                                                  const QSqlQuery &query) const;
};

#endif // DBDATAEXPORT_H
