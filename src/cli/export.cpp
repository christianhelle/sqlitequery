#include "export.h"
#include "../../database/dbanalyzer.h"

void Export::exportDataToCsvFile(const QString &file,
                                 const QString &outputFolder,
                                 const QString &delimiter,
                                 const bool showProgress) {
    const auto dataExportProgress = std::make_unique<ExportDataProgress>();
    if (showProgress) {
        dataExportProgress->setShowProgress(true);
    } else {
        dataExportProgress->setShowProgress(false);
    }
    const auto progress = dataExportProgress.get();

    const auto tcs = std::make_unique<CancellationTokenSource>();
    const auto cancellationToken = tcs->get();

    const auto database = std::make_unique<Database>();
    database->setSource(file);
    if (!database->open()) {
        qWarning("Unable to open file");
        return;
    }

    const auto analyzer = std::make_unique<DbAnalyzer>(database.get());
    DatabaseInfo info;
    analyzer->analyze(info);

    DbDataExport dataExport(info);
    dataExport.exportDataToCsvFile(database.get(),
                                   outputFolder,
                                   delimiter,
                                   &cancellationToken,
                                   progress);

    printf("\nExported %llu row(s)\n", progress->getAffectedRows());
}
