#ifndef IEXPORTORCHESTRATOR_H
#define IEXPORTORCHESTRATOR_H

#include <QString>
#include <memory>
#include "progress.h"
#include "cancellation.h"
#include "idatabase.h"
#include "idataexport.h"

class IExportOrchestrator {
public:
    virtual ~IExportOrchestrator() = default;

    virtual void exportToSql(IDatabase *database,
                              const DatabaseInfo &info,
                              const QString &filepath,
                              std::unique_ptr<ExportDataProgress> progress) = 0;

    virtual void exportToCsv(IDatabase *database,
                              const DatabaseInfo &info,
                              const QString &outputFolder,
                              const QString &delimiter,
                              std::unique_ptr<ExportDataProgress> progress) = 0;
};

#endif // IEXPORTORCHESTRATOR_H
