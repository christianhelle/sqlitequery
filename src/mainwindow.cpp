#include "mainwindow.h"
#include "recentfiles.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "dbexport.h"
#include "mainthread.h"

#include <QMessageBox>
#include <QSqlTableModel>
#include <QTreeWidget>
#include <QTableView>
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <thread>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
    qDebug("MainWindow::MainWindow(QWidget*)");

    ui->setupUi(this);
    ui->splitterMain->setStretchFactor(1, 3);
    ui->splitterQueryTab->setStretchFactor(1, 1);
    this->setWindowTitle("SQLite Query Analyzer");

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(createNewFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openExistingFile()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveSql()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(appExit()));
    connect(ui->actionExecute_Query, SIGNAL(triggered()), this, SLOT(executeQuery()));
    connect(ui->actionShrink, SIGNAL(triggered()), this, SLOT(shrink()));
    connect(ui->actionScript_Schema, SIGNAL(triggered()), this, SLOT(scriptSchema()));
    connect(ui->actionScript_Data, SIGNAL(triggered()), this, SLOT(scriptData()));
    connect(ui->actionCancel, SIGNAL(triggered()), this, SLOT(cancel()));
    connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this,
            SLOT(treeNodeChanged(QTreeWidgetItem*,int)));
    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*)), this,
            SLOT(treeNodeChanged(QTreeWidgetItem*)));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionRefresh, SIGNAL(triggered()), this, SLOT(refreshDatabase()));

    this->database = new Database();
    this->analyzer = new DbAnalyzer(database);
    this->query = new DbQuery(ui->queryResultsGrid, this->database);

    this->tree = new DbTree(ui->treeWidget);
    this->highlighter = new Highlighter(ui->textEdit->document());

    // ReSharper disable once CppDFAMemoryLeak - By design
    this->recentFilesMenu = new QMenu("Recent Files");
    ui->menuFile->insertMenu(ui->actionSave, recentFilesMenu);

    Settings::init();
    this->loadRecentFiles();
    // this->openExistingFile();
}

MainWindow::~MainWindow() {
    qDebug("MainWindow::~MainWindow()");

    delete analyzer;
    delete highlighter;
    delete query;
    delete tree;
    delete recentFilesMenu;
    delete ui;
}

void MainWindow::loadRecentFiles() const {
    QStringList files = RecentFiles::getList();
    if (files.empty())
        return;

    if (!this->recentFilesMenu->actions().empty())
        this->recentFilesMenu->clear();

    foreach(const QString file, files) {
        QAction *action = recentFilesMenu->addAction(file);
        action->setObjectName(file);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(openRecentFile()));
    }
}

void MainWindow::openRecentFile() const {
    const QString file = sender()->objectName();
    this->openDatabase(file);
}

QString MainWindow::showFileDialog(const QFileDialog::AcceptMode mode) {
    QFileDialog dialog(this);
    dialog.setAcceptMode(mode);
    dialog.setDirectory(QDir::home());
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec()) {
        QStringList files = dialog.selectedFiles();
        if (files.length())
            return files.first();
    }

    qDebug("File dialog cancelled");
    return Q_NULLPTR;
}

void MainWindow::createNewFile() {
    if (this->dataExportProgress.get() != nullptr) {
        ui->queryResultMessagesTextEdit->setPlainText("Unable to process request. Data export in progress");
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }

    const QString filepath = this->showFileDialog(QFileDialog::AcceptSave);
    this->openDatabase(filepath);
    RecentFiles::add(filepath);
    this->loadRecentFiles();
}

void MainWindow::openDatabase(const QString &filename) const {
    if (this->dataExportProgress.get() != nullptr) {
        ui->queryResultMessagesTextEdit->setPlainText("Unable to process request. Data export in progress");
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }

    this->database->setSource(filename);
    if (!this->database->open()) {
        qDebug("Unable to open file");
        return;
    }

    this->analyzeDatabase();
    RecentFiles::add(filename);

    this->query->clearResults();
    ui->queryResultMessagesTextEdit->clear();
    ui->tabWidget->setCurrentIndex(0);
    ui->textEdit->clear();

    if (ui->tableView->model() != Q_NULLPTR) {
        std::make_unique<QSqlTableModel>(ui->tableView->model());
        ui->tableView->setModel(Q_NULLPTR);
    }
}

void MainWindow::openExistingFile() {
    if (this->dataExportProgress.get() != nullptr) {
        ui->queryResultMessagesTextEdit->setPlainText("Unable to process request. Data export in progress");
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }
    QString filepath = this->showFileDialog(QFileDialog::AcceptOpen);
    this->openDatabase(filepath);
    RecentFiles::add(filepath);
    this->loadRecentFiles();
}

void MainWindow::appExit() {
    exit(0);
}

void MainWindow::shrink() const {
    if (this->dataExportProgress.get() != nullptr) {
        ui->queryResultMessagesTextEdit->setPlainText("Unable to process request. Data export in progress");
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }
    const QString filename = this->database->getFilename();
    if (filename.isNull() || filename.isEmpty())
        return;

    this->database->shrink();
    this->analyzeDatabase();
}

void MainWindow::refreshDatabase() const {
    this->analyzeDatabase();
}

void MainWindow::analyzeDatabase() const {
    if (this->dataExportProgress.get() != nullptr) {
        ui->queryResultMessagesTextEdit->setPlainText("Unable to process request. Data export in progress");
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }

    DatabaseInfo info;
    if (!analyzer->analyze(info)) {
        qDebug("Analyze database failed");
        return;
    }

    this->tree->populateTree(info);

    this->database->close();
}

void MainWindow::executeQuery() const {
    if (this->dataExportProgress.get() != nullptr) {
        ui->queryResultMessagesTextEdit->setPlainText("Unable to process request. Data export in progress");
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }

    QStringList list(ui->textEdit->toPlainText().split(";", Qt::SkipEmptyParts));
    QStringList errors;

    QElapsedTimer time;
    time.start();

    if (this->query->execute(list, &errors)) {
        ui->tabWidget->setCurrentIndex(0);
        ui->queryResultTab->setCurrentIndex(0);
    }

    const auto milliseconds = static_cast<double>(time.elapsed());
    const auto msg = "Query execution took " + QString::number(milliseconds / 1000) + " seconds";
    ui->queryResultMessagesTextEdit->setPlainText(msg);

    foreach(const QString sql, list) {
        if (sql.contains("create", Qt::CaseInsensitive) ||
            sql.contains("drop", Qt::CaseInsensitive) ||
            sql.contains("insert", Qt::CaseInsensitive) ||
            sql.contains("delete", Qt::CaseInsensitive)) {
            ui->queryResultTab->setCurrentIndex(1);
            analyzeDatabase();
            break;
        }
    }
}

void MainWindow::scriptSchema() const {
    DatabaseInfo info;
    analyzer->analyze(info);
    const auto exporter = std::make_unique<DbExport>(info);
    const auto schema = exporter->exportSchema();
    ui->textEdit->setPlainText(schema);
}

void MainWindow::setEnabledActions(const bool enabled) {
    ui->actionRefresh->setEnabled(enabled);
    ui->actionShrink->setEnabled(enabled);
    ui->actionScript_Data->setEnabled(enabled);
    ui->actionExecute_Query->setEnabled(enabled);
    ui->actionScript_Schema->setEnabled(enabled);
    ui->actionCancel->setVisible(!enabled);
    if (enabled) {
        this->dataExportProgress.release();
    }
}

void MainWindow::scriptData() {
    const QString filepath = this->showFileDialog(QFileDialog::AcceptSave);
    if (filepath.isEmpty())
        return;

    DatabaseInfo info;
    analyzer->analyze(info);
    this->setEnabledActions(false);
    ui->queryResultTab->setCurrentIndex(1);

    this->dataExportProgress = std::make_unique<ExportDataProgress>();
    const auto progress = dataExportProgress.get();

    this->tcs = std::make_unique<CancellationTokenSource>();
    const auto cancellationToken = tcs->get();

    auto future = QtConcurrent::run([this, info, filepath, cancellationToken, progress]() {
        const auto exporter = std::make_unique<DbExport>(info);
        exporter->exportDataToFile(database, filepath, &cancellationToken, progress);
        progress->reset();
    });
    future.then([this, cancellationToken, progress]() {
        MainThread::run([this, cancellationToken, progress]() {
            this->setEnabledActions(true);
            if (cancellationToken.isCancellationRequested())
                ui->queryResultMessagesTextEdit->setPlainText(
                    "Data export cancelled. Exported " +
                    QString("%1 row(s)").arg(progress->getAffectedRows()));
        });
    });

    auto _ = QtConcurrent::run([this, cancellationToken, progress]() {
        do {
          std::this_thread::sleep_for(std::chrono::seconds(1));
          MainThread::run([this, progress]() {
            ui->queryResultMessagesTextEdit->setPlainText(
              "Data export in-progress. Exported " +
              QString("%1 row(s)").arg(progress->getAffectedRows()));
          });
        } while (progress->getAffectedRows() > 0 && !cancellationToken.isCancellationRequested());

        MainThread::run([this, progress]() {
          ui->queryResultMessagesTextEdit->setPlainText("");
        });
    });
}

void MainWindow::cancel() const {
    this->tcs->cancel();
}

void MainWindow::saveSql() {
    const auto sql = ui->textEdit->toPlainText();
    if (sql.isEmpty())
        return;
    const QString filepath = this->showFileDialog(QFileDialog::AcceptSave);
    const auto file = std::make_unique<QFile>(filepath);
    if (file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(file.get());
        out << sql;
        file->close();
    }
}

void MainWindow::treeNodeChanged(QTreeWidgetItem *item) const {
    treeNodeChanged(item, 0);
}

void MainWindow::treeNodeChanged(QTreeWidgetItem *item, const int column) const {
    if (this->dataExportProgress.get() != nullptr) {
        ui->queryResultMessagesTextEdit->setPlainText(
            "Unable to process request. Data export in progress - " + QString("%1 row(s)").arg(
                this->dataExportProgress.get()->getAffectedRows()));
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }
    if (item && item->type() == QTreeWidgetItem::UserType + 1) {
        qDebug("table selected");

        if (!this->database->open()) {
            qDebug("Unable to open database");
            return;
        }

        if (ui->tableView->model() != Q_NULLPTR)
            std::make_unique<QSqlTableModel>(ui->tableView->model());

        // The model is no longer alive after the unique pointer destroys it...
        // ReSharper disable once CppDFAMemoryLeak
        const auto model = new QSqlTableModel(nullptr, this->database->getDatabase());
        model->setTable(item->text(column));
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
        model->select();

        ui->tableView->setModel(model);
        ui->tableView->setSortingEnabled(true);
        ui->tabWidget->setCurrentIndex(1);
    }
}

void MainWindow::about() {
    const QString text = "SQLite Query Analyzer\n"
                         "Version: " + QApplication::applicationVersion() + "\n"
                         "Copyright (c) Christian Resma Helle 2015\n"
                         "All rights reserved.\n\n"
                         "Description:\n"
                         "A fast and lightweight cross-platform GUI tool "
                         "for querying and manipulating SQLite databases.";
    QMessageBox::about(this, "About", text);
}
