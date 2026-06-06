#ifndef DATAEXPORT_H
#define DATAEXPORT_H

#include "idataexport.h"
#include "databaseinfo.h"
#include "idatabase.h"

class DataExport : public IDataExport {
public:
    explicit DataExport(DatabaseInfo info);

    void exportToSqlFile(IDatabase *database,
                         const QString &filename,
                         const CancellationToken *cancellationToken,
                         ExportDataProgress *progress) const override;

    void exportToCsvFile(IDatabase *database,
                         const QString &outputFolder,
                         const QString &delimiter,
                         const CancellationToken *cancellationToken,
                         ExportDataProgress *progress) const override;

private:
    DatabaseInfo info;
    static QStringList getColumnDefs(const Table &table);
    QStringList getColumnValueDefs(const Table &table,
                                    const QSqlQuery &query) const;
    [[nodiscard]] QStringList getTextTypes() const;
    static bool isInternalTable(const Table &table);
};

#endif // DATAEXPORT_H
