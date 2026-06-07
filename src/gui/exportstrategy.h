#ifndef EXPORTSTRATEGY_H
#define EXPORTSTRATEGY_H

#include "../database/sqlitedatabase.h"
#include "../database/databaseinfo.h"
#include "../threading/cancellation.h"
#include "../database/progress.h"

class ExportStrategy {
public:
    virtual ~ExportStrategy() = default;
    virtual void execute(SqliteDatabase *db, const CancellationToken *token, ExportDataProgress *progress) = 0;
};

class SqlExportStrategy : public ExportStrategy {
public:
    SqlExportStrategy(DatabaseInfo info, QString filepath);
    void execute(SqliteDatabase *db, const CancellationToken *token, ExportDataProgress *progress) override;
private:
    DatabaseInfo info_;
    QString filepath_;
};

class CsvExportStrategy : public ExportStrategy {
public:
    CsvExportStrategy(DatabaseInfo info, QString outputFolder, QString delimiter);
    void execute(SqliteDatabase *db, const CancellationToken *token, ExportDataProgress *progress) override;
private:
    DatabaseInfo info_;
    QString outputFolder_;
    QString delimiter_;
};

#endif // EXPORTSTRATEGY_H
