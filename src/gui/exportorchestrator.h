#ifndef EXPORTORCHESTRATOR_H
#define EXPORTORCHESTRATOR_H

#include <functional>
#include <QObject>
#include <QString>
#include "../threading/cancellation.h"
#include "../threading/mainthread.h"
#include "../database/databaseinfo.h"
#include "../database/progress.h"

class IDatabase;

class ExportOrchestrator : public QObject {
    Q_OBJECT

public:
    explicit ExportOrchestrator(QObject *parent = nullptr);
    ~ExportOrchestrator() override;

    void exportToSql(DatabaseInfo info, QString filePath, IDatabase *db);
    void exportToCsv(DatabaseInfo info, QString outputFolder, QString delimiter, IDatabase *db);
    void cancel();

    bool isExporting() const { return progress_ != nullptr; }
    ExportDataProgress *getProgress() const { return progress_.get(); }

signals:
    void exportProgress(uint64_t rowsExported);
    void exportCompleted(uint64_t rowsExported);
    void exportCancelled();

private slots:
    void onExportComplete();

private:
    void startProgressPolling(CancellationToken token);
    void runExport(IDatabase *db, DatabaseInfo info,
                   std::function<void(IDatabase *, const DatabaseInfo &, const CancellationToken *, ExportDataProgress *)> fn);

    std::unique_ptr<ExportDataProgress> progress_;
    std::unique_ptr<CancellationTokenSource> tcs_;
};

#endif // EXPORTORCHESTRATOR_H
