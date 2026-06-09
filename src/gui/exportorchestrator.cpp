#include "exportorchestrator.h"
#include "../database/dbexportdata.h"
#include <QtConcurrent/QtConcurrent>

#include <chrono>
#include <thread>

ExportOrchestrator::ExportOrchestrator(QObject *parent)
    : QObject(parent) {
}

ExportOrchestrator::~ExportOrchestrator() {
    cancel();
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
    tearingDown_ = true;
    uint64_t rows = progress_->getAffectedRows();
    completed_ = true;
    progress_.reset();
    tcs_.reset();
    emit exportCompleted(rows);
}

void ExportOrchestrator::startProgressPolling(CancellationToken token) {
    QPointer<ExportOrchestrator> guard(this);
    auto _ = QtConcurrent::run([this, token, guard]() {
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (completed_ || tearingDown_)
                break;
            MainThread::run([guard]() {
                if (!guard || guard->tearingDown_)
                    return;
                emit guard->exportProgress(guard->progress_->getAffectedRows());
            });
        } while (!token.isCancellationRequested() &&
                 !progress_->isCompleted());
    });
}

void ExportOrchestrator::runExport(IDatabase *db, DatabaseInfo info,
                                   std::function<void(IDatabase *, const DatabaseInfo &, const CancellationToken *, ExportDataProgress *)> fn) {
    progress_ = std::make_unique<ExportDataProgress>();
    tcs_ = std::make_unique<CancellationTokenSource>();
    completed_ = false;
    CancellationToken token = tcs_->get();
    ExportDataProgress *progressPtr = progress_.get();

    auto future = QtConcurrent::run([db, info, fn, token, progressPtr]() {
        fn(db, info, const_cast<const CancellationToken*>(&token), progressPtr);
    });

    future.then([this]() {
        QPointer<ExportOrchestrator> guard(this);
        MainThread::run([guard]() {
            if (!guard || guard->tearingDown_)
                return;
            guard->onExportComplete();
        });
    });

    startProgressPolling(token);
}
