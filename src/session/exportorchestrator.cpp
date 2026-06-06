#include "exportorchestrator.h"

void ExportOrchestrator::exportToSql(IDatabase *database,
                                      const DatabaseInfo &info,
                                      const QString &filepath,
                                      std::unique_ptr<ExportDataProgress> progress) {
    CancellationTokenSource tcs;
    auto cancellationToken = tcs.get();
    auto future = QtConcurrent::run([database, info, filepath, cancellationToken, progress = std::move(progress)]() {
        DataExport exporter(info);
        exporter.exportToSqlFile(database, filepath, &cancellationToken, progress.get());
    });
    Q_UNUSED(future)
}

void ExportOrchestrator::exportToCsv(IDatabase *database,
                                      const DatabaseInfo &info,
                                      const QString &outputFolder,
                                      const QString &delimiter,
                                      std::unique_ptr<ExportDataProgress> progress) {
    CancellationTokenSource tcs;
    auto cancellationToken = tcs.get();
    auto future = QtConcurrent::run([database, info, outputFolder, delimiter, cancellationToken, progress = std::move(progress)]() {
        DataExport exporter(info);
        exporter.exportToCsvFile(database, outputFolder, delimiter, &cancellationToken, progress.get());
    });
    Q_UNUSED(future)
}
