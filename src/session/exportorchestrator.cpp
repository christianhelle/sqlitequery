#include "exportorchestrator.h"

void ExportOrchestrator::exportToSql(IDatabase *database,
                                      const DatabaseInfo &info,
                                      const QString &filepath,
                                      std::unique_ptr<ExportDataProgress> progress) {
    auto future = QtConcurrent::run([database, info, filepath, progress]() {
        DataExport exporter(info);
        CancellationToken dummy;
        exporter.exportToSqlFile(database, filepath, &dummy, progress.get());
    });
    Q_UNUSED(future)
}

void ExportOrchestrator::exportToCsv(IDatabase *database,
                                      const DatabaseInfo &info,
                                      const QString &outputFolder,
                                      const QString &delimiter,
                                      std::unique_ptr<ExportDataProgress> progress) {
    auto future = QtConcurrent::run([database, info, outputFolder, delimiter, progress]() {
        DataExport exporter(info);
        CancellationToken dummy;
        exporter.exportToCsvFile(database, outputFolder, delimiter, &dummy, progress.get());
    });
    Q_UNUSED(future)
}
