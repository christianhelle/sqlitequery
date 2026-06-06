#ifndef EXPORTORCHESTRATOR_H
#define EXPORTORCHESTRATOR_H

#include "iexportorchestrator.h"
#include "../database/dataexport.h"
#include <QtConcurrent/QtConcurrent>
#include "../threading/mainthread.h"

class ExportOrchestrator : public IExportOrchestrator {
public:
    void exportToSql(IDatabase *database,
                     const DatabaseInfo &info,
                     const QString &filepath,
                     std::unique_ptr<ExportDataProgress> progress) override;

    void exportToCsv(IDatabase *database,
                     const DatabaseInfo &info,
                     const QString &outputFolder,
                     const QString &delimiter,
                     std::unique_ptr<ExportDataProgress> progress) override;
};

#endif // EXPORTORCHESTRATOR_H
