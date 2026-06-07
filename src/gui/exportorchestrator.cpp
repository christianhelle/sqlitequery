#include "exportorchestrator.h"
#include <QtConcurrent/QtConcurrent>

#include <QElapsedTimer>
#include <chrono>
#include <thread>

ExportOrchestrator::ExportOrchestrator(QObject *parent)
    : QObject(parent) {
}

ExportOrchestrator::~ExportOrchestrator() {
    cancel();
}

void ExportOrchestrator::startExport(std::unique_ptr<ExportStrategy> strategy, SqliteDatabase *db) {
    progress_ = std::make_unique<ExportDataProgress>();
    tcs_ = std::make_unique<CancellationTokenSource>();
    auto token = tcs_->get();

    auto future = QtConcurrent::run([this, strategy = std::move(strategy), db, token]() {
        strategy->execute(db, const_cast<const CancellationToken*>(&token), progress_.get());
    });

    future.then([this]() {
        MainThread::run([this]() {
            this->onExportComplete();
        });
    });

    startProgressPolling(token);
}

void ExportOrchestrator::cancel() {
    if (tcs_ && !tcs_->get().isCancellationRequested()) {
        tcs_->cancel();
    }
}

void ExportOrchestrator::onExportComplete() {
    uint64_t rows = progress_->getAffectedRows();
    progress_.release();
    tcs_.release();
    emit exportCompleted(rows);
}

void ExportOrchestrator::startProgressPolling(CancellationToken token) {
    auto _ = QtConcurrent::run([this, token]() {
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            MainThread::run([this]() {
                emit exportProgress(progress_->getAffectedRows());
            });
        } while (!token.isCancellationRequested() &&
                 !progress_->isCompleted());
    });
}
