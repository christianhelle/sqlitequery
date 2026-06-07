#ifndef EXPORTORCHESTRATOR_H
#define EXPORTORCHESTRATOR_H

#include <QObject>
#include "../threading/cancellation.h"
#include "../threading/mainthread.h"
#include "../database/progress.h"
#include "exportstrategy.h"

class ExportOrchestrator : public QObject {
    Q_OBJECT

public:
    explicit ExportOrchestrator(QObject *parent = nullptr);
    ~ExportOrchestrator() override;

    void startExport(std::unique_ptr<ExportStrategy> strategy, Database *db);
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

    std::unique_ptr<ExportDataProgress> progress_;
    std::unique_ptr<CancellationTokenSource> tcs_;
};

#endif // EXPORTORCHESTRATOR_H
