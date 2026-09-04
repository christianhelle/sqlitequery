#include "exportorchestrator.h"
#include "../database/dbexportdata.h"
#include <QPointer>
#include <QtConcurrent/QtConcurrent>

#include <chrono>
#include <thread>

ExportOrchestrator::ExportOrchestrator(QObject *parent)
    : QObject(parent) {
}

ExportOrchestrator::~ExportOrchestrator() {
    cancel();

    // Both tasks read members of this object, so neither may outlive it. cancel()
    // above makes them stop at their next check rather than at the end of the
    // export. Queued progress callbacks are guarded separately by a QPointer.
    completed_ = true;
    exportTask_.waitForFinished();
    pollingTask_.waitForFinished();
}

void ExportOrchestrator::exportToSql(DatabaseInfo info, QString filePath, IDatabase *db) {
    auto fn = [filePath](IDatabase *db, const DatabaseInfo &info, const CancellationToken *token, ExportDataProgress *progress) {
        DbDataExport exporter(info);
        exporter.exportDataToSqlFile(db, filePath, token, progress);
    };
    runExport(db, std::move(info), std::move(fn));
}

void ExportOrchestrator::exportToCsv(DatabaseInfo info, QString outputFolder, QString delimiter, IDatabase *db) {
    auto fn = [outputFolder, delimiter](IDatabase *db, const DatabaseInfo &info, const CancellationToken *token, ExportDataProgress *progress) {
        DbDataExport exporter(info);
        exporter.exportDataToCsvFile(db, outputFolder, delimiter, token, progress);
    };
    runExport(db, std::move(info), std::move(fn));
}

void ExportOrchestrator::cancel() {
    if (tcs_ && !tcs_->get().isCancellationRequested()) {
        tcs_->cancel();
    }
}

void ExportOrchestrator::onExportComplete() {
    const uint64_t rows = progress_->getAffectedRows();
    completed_ = true;
    progress_.reset();
    tcs_.reset();
    emit exportCompleted(rows);
}

void ExportOrchestrator::startProgressPolling(CancellationToken token) {
    // The polling thread outlives the export, so it keeps the progress and
    // cancellation state alive through shared ownership instead of raw pointers.
    auto progress = progress_;
    auto tcs = tcs_;
    const QPointer<ExportOrchestrator> self(this);
    pollingTask_ = QtConcurrent::run([this, self, token, progress, tcs]() {
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (completed_)
                break;
            const uint64_t rows = progress->getAffectedRows();
            // Queued onto the GUI thread, so it can run after this object is
            // gone; QPointer clears itself when that happens.
            MainThread::run([self, rows]() {
                if (!self.isNull())
                    emit self->exportProgress(rows);
            });
        } while (!token.isCancellationRequested() &&
                 !progress->isCompleted());
    });
}

void ExportOrchestrator::runExport(IDatabase *db, DatabaseInfo info,
                                   std::function<void(IDatabase *, const DatabaseInfo &, const CancellationToken *, ExportDataProgress *)> fn) {
    progress_ = std::make_shared<ExportDataProgress>();
    tcs_ = std::make_shared<CancellationTokenSource>();
    completed_ = false;
    CancellationToken token = tcs_->get();
    ExportDataProgress *progressPtr = progress_.get();

    exportTask_ = QtConcurrent::run([db, info = std::move(info), fn = std::move(fn), token, progressPtr]() {
        fn(db, info, &token, progressPtr);
    });

    const QPointer<ExportOrchestrator> self(this);
    exportTask_.then([self]() {
        MainThread::run([self]() {
            if (!self.isNull())
                self->onExportComplete();
        });
    });

    startProgressPolling(token);
}
