#include "exportstrategy.h"
#include "../database/dbexportdata.h"

SqlExportStrategy::SqlExportStrategy(DatabaseInfo info, QString filepath)
    : info_(std::move(info)), filepath_(std::move(filepath)) {
}

void SqlExportStrategy::execute(IDatabase *db, const CancellationToken *token, ExportDataProgress *progress) {
    DbDataExport exporter(info_);
    exporter.exportDataToSqlFile(db, filepath_, token, progress);
}

CsvExportStrategy::CsvExportStrategy(DatabaseInfo info, QString outputFolder, QString delimiter)
    : info_(std::move(info)), outputFolder_(std::move(outputFolder)), delimiter_(std::move(delimiter)) {
}

void CsvExportStrategy::execute(IDatabase *db, const CancellationToken *token, ExportDataProgress *progress) {
    DbDataExport exporter(info_);
    exporter.exportDataToCsvFile(db, outputFolder_, delimiter_, token, progress);
}
