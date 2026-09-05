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
    // The CSV header row: Column names as the user wrote them. A CSV header
    // is not SQL, so these are not Identifiers.
    static QStringList columnNames(const Table &table);

    // The column list of an INSERT statement, where the same names are
    // Identifiers and have to be delimited.
    static QStringList quotedColumnNames(const Table &table);

    [[nodiscard]] QList<bool> getTextColumnFlags(const Table &table) const;

    static QStringList getColumnValueDefs(const QList<bool> &isTextColumn,
                                          const QList<QVariant> &values);
};

#endif // DBDATAEXPORT_H
