#ifndef IDATAEXPORT_H
#define IDATAEXPORT_H

#include "idatabase.h"
#include "cancellation.h"
#include "progress.h"

class IDataExport {
public:
    virtual ~IDataExport() = default;

    virtual void exportToSqlFile(IDatabase *database,
                                  const QString &filename,
                                  const CancellationToken *cancellationToken,
                                  ExportDataProgress *progress) const = 0;

    virtual void exportToCsvFile(IDatabase *database,
                                  const QString &outputFolder,
                                  const QString &delimiter,
                                  const CancellationToken *cancellationToken,
                                  ExportDataProgress *progress) const = 0;
};

#endif // IDATAEXPORT_H
