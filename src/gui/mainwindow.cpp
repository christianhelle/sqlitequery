#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../settings/settings.h"
#include "../database/dbexportschema.h"
#include "prompts.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QSqlTableModel>
#include <QTreeWidget>
#include <QStatusBar>
#include <QTableView>
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <thread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    ui = std::make_unique<Ui::MainWindow>();
    ui->setupUi(this);
    ui->splitterMain->setStretchFactor(1, 3);
    ui->splitterQueryTab->setStretchFactor(1, 1);
    this->setWindowTitle("SQLite Query Analyzer");
    this->connectSignalSlots();

    this->database = std::make_unique<SqliteDatabase>();
    this->analyzer = std::make_unique<DbAnalyzer>(database.get());
    this->executor = std::make_unique<QueryExecutor>(database.get());
    this->queryPresenter = std::make_unique<QueryExecutionPresenter>(ui->queryResultsGrid, executor.get());

    this->tree = std::make_unique<DbTree>(ui->treeWidget);
    this->highlighter = std::make_unique<Highlighter>(ui->textEdit->document());

    this->sessionManager = std::make_unique<SessionManager>(this);
    this->exportOrchestrator = std::make_unique<ExportOrchestrator>(this);

    this->recentFilesMenu = std::make_unique<QMenu>("Recent Files");
    ui->menuFile->insertMenu(ui->actionSave, recentFilesMenu.get());

    this->statusBar = std::make_unique<QStatusBar>(this);
    this->setStatusBar(statusBar.get());

    sessionManager->init();
    sessionManager->loadRecentFiles(recentFilesMenu.get(), this);
    restoreWindowState();

    loaded = true;
}

MainWindow::~MainWindow() {
    saveSession();
    saveWindowState(this->window()->size());
    this->queryPresenter.reset();
    this->executor.reset();
    this->tree->clear();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    auto const keys = event->keyCombination();
    auto const modifiers = keys.keyboardModifiers();
    if (keys.key() == Qt::Key_T && modifiers == Qt::ControlModifier) {
        ui->treeWidget->setFocus();
    } else if (keys.key() == Qt::Key_E && modifiers == Qt::ControlModifier) {
        ui->tabWidget->setCurrentIndex(0);
        ui->textEdit->setFocus();
    } else if (keys.key() == Qt::Key_D && modifiers == Qt::ControlModifier) {
        ui->tabWidget->setCurrentIndex(1);
        ui->tableView->setFocus();
    } else if (keys.key() == Qt::Key_Q && modifiers == Qt::ControlModifier) {
        this->appExit();
    } else if (keys.key() == Qt::Key_Delete) {
        deleteSelectedTable();
    }
}

void MainWindow::deleteSelectedTable() {
    const auto indexes = ui->treeWidget->selectionModel()->selectedIndexes();
    if (indexes.size() <= 0)
        return;

    const auto item = ui->treeWidget->itemFromIndex(indexes.at(0));
    if (item->parent() == nullptr || item->parent()->text(0) != "Tables")
        return;

    const auto tableName = item->text(0);
    if (!Prompts::confirmDelete(this, tableName)) {
        return;
    }

    const auto list = QStringList() << "DROP TABLE " + tableName;
    QStringList errors;
    QElapsedTimer time;
    time.start();

    const auto deleted = this->queryPresenter->execute(list, &errors);
    const auto milliseconds = static_cast<double>(time.elapsed());
    const auto msg = "Query execution took " + QString::number(milliseconds / 1000) + " seconds";
    this->showMessage(msg);

    if (!deleted) {
        const auto errorMessage = errors.join("\r\n");
        ui->queryResultMessagesTextEdit->setPlainText(errorMessage);
        Prompts::showError(this, errorMessage);
    } else {
        this->analyzeDatabase();
    }
}

void MainWindow::connectSignalSlots() const {
    connect(ui->actionNew,
            SIGNAL(triggered()),
            this,
            SLOT(createNewFile()));
    connect(ui->actionOpen,
            SIGNAL(triggered()),
            this,
            SLOT(openExistingFile()));
    connect(ui->actionSave,
            SIGNAL(triggered()),
            this,
            SLOT(saveSql()));
    connect(ui->actionExit,
            SIGNAL(triggered()),
            this,
            SLOT(appExit()));
    connect(ui->actionExecute_Query,
            SIGNAL(triggered()),
            this,
            SLOT(executeQuery()));
    connect(ui->actionShrink,
            SIGNAL(triggered()),
            this,
            SLOT(shrink()));
    connect(ui->actionScript_Schema,
            SIGNAL(triggered()),
            this,
            SLOT(scriptSchema()));
    connect(ui->actionScript_SQL,
            SIGNAL(triggered()),
            this,
            SLOT(exportDataToSqlScript()));
    connect(ui->actionScript_CSV,
            SIGNAL(triggered()),
            this,
            SLOT(exportDataToCsvFiles()));
    connect(ui->actionCancel,
            SIGNAL(triggered()),
            this,
            SLOT(cancel()));
    connect(ui->treeWidget,
            SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this,
            SLOT(treeNodeChanged(QTreeWidgetItem*,int)));
    connect(ui->treeWidget,
            SIGNAL(currentItemChanged(QTreeWidgetItem*)),
            this,
            SLOT(treeNodeChanged(QTreeWidgetItem*)));
    connect(ui->actionAbout,
            SIGNAL(triggered()),
            this,
            SLOT(about()));
    connect(ui->actionRefresh,
            SIGNAL(triggered()),
            this,
            SLOT(refreshDatabase()));
}

void MainWindow::restoreWindowState() {
    WindowState windowState;
    sessionManager->restoreWindowState(&windowState);
    this->resize(windowState.size);

    if (windowState.position.x() > 0 &&
        windowState.position.y() > 0)
        this->move(windowState.position);

    if (windowState.treeWidth > 0 &&
        windowState.tabWidth > 0) {
        ui->splitterMain->setSizes({
                windowState.treeWidth,
                windowState.tabWidth
        });
    }

    if (windowState.queryTextHeight > 0 &&
        windowState.queryResultHeight > 0)
        ui->splitterQueryTab->setSizes({
                windowState.queryTextHeight,
                windowState.queryResultHeight
        });
}

void MainWindow::saveWindowState(const QSize &size) const {
    if (!this->loaded)
        return;

    const auto windowSize = size;
    const auto position = this->window()->pos();
    const int treeWidth = ui->splitterMain->sizes().first();
    const int tabWidth = ui->splitterMain->sizes().last();
    const int queryTextHeight = ui->splitterQueryTab->sizes().first();
    const int queryResultHeight = ui->splitterQueryTab->sizes().last();

    WindowState state;
    state.size = windowSize;
    state.position = position;
    state.treeWidth = treeWidth;
    state.tabWidth = tabWidth;
    state.queryTextHeight = queryTextHeight;
    state.queryResultHeight = queryResultHeight;
    sessionManager->saveWindowState(state);
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    saveWindowState(e->size());
}

void MainWindow::openRecentFile() {
    const auto *senderObject = sender();
    if (senderObject == nullptr) {
        return;
    }
    const QString file = senderObject->objectName();
    this->openDatabase(file);
}

void MainWindow::restoreLastSession() {
    SessionState state;
    sessionManager->restoreSession(&state);
    if (!state.sqliteFile.isEmpty()) {
        this->openDatabase(state.sqliteFile);
        ui->textEdit->setPlainText(state.query);
    }
}

void MainWindow::saveSession() const {
    sessionManager->saveSession(this->database->getFilename(), ui->textEdit->toPlainText());
}

void MainWindow::createNewFile() {
    if (exportOrchestrator->isExporting()) {
        const auto msg = "Unable to process request. Data export in progress";
        this->statusBar->showMessage(msg, 5000);
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }

    const QString filepath = Prompts::getFilePath(this, QFileDialog::AcceptSave);
    this->openDatabase(filepath);
    sessionManager->addRecentFile(filepath);
}

void MainWindow::openDatabase(const QString &filename) {
    if (exportOrchestrator->isExporting()) {
        const auto msg = "Unable to process request. Data export in progress";
        this->statusBar->showMessage(msg, 5000);
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }

    if (!this->database->getFilename().isEmpty()) {
        this->queryPresenter->clearResults();
    }

    this->database->setSource(filename);
    if (!this->database->open()) {
        return;
    }

    this->analyzeDatabase();
    sessionManager->addRecentFile(filename);

    ui->queryResultMessagesTextEdit->clear();
    ui->tabWidget->setCurrentIndex(0);
    ui->textEdit->clear();

    if (ui->tableView->model() != Q_NULLPTR) {
        const auto model = dynamic_cast<QSqlTableModel *>(ui->tableView->model());
        model->clear();
        ui->tableView->setModel(Q_NULLPTR);
        delete model;
    }

    this->setWindowTitle("SQLite Query Analyzer - " + filename);
}

void MainWindow::openExistingFile() {
    if (exportOrchestrator->isExporting()) {
        const auto msg = "Unable to process request. Data export in progress";
        this->statusBar->showMessage(msg, 5000);
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }
    const auto filepath = Prompts::getFilePath(this, QFileDialog::AcceptOpen);
    this->openDatabase(filepath);
    sessionManager->addRecentFile(filepath);
}

void MainWindow::appExit() const {
    this->saveSession();
    this->saveWindowState(this->window()->size());
    exit(0);
}

void MainWindow::shrink() const {
    if (exportOrchestrator->isExporting()) {
        const auto msg = "Unable to process request. Data export in progress";
        this->statusBar->showMessage(msg, 5000);
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }
    if (const QString filename = this->database->getFilename();
        filename.isNull() || filename.isEmpty())
        return;

    this->database->shrink();
    this->analyzeDatabase();
}

void MainWindow::refreshDatabase() const {
    this->analyzeDatabase();
}

void MainWindow::analyzeDatabase() const {
    if (exportOrchestrator->isExporting()) {
        const auto msg = "Unable to process request. Data export in progress";
        this->statusBar->showMessage(msg, 5000);
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }

    DatabaseInfo info;
    if (!analyzer->analyze(info)) {
        return;
    }

    this->tree->populateTree(info);

    this->database->close();
}

void MainWindow::executeQuery() const {
    if (exportOrchestrator->isExporting()) {
        const auto msg = "Unable to process request. Data export in progress";
        this->statusBar->showMessage(msg, 5000);
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }

    QStringList list(ui->textEdit->toPlainText().split(";", Qt::SkipEmptyParts));
    QStringList errors;

    QElapsedTimer time;
    time.start();

    if (this->queryPresenter->execute(list, &errors)) {
        ui->tabWidget->setCurrentIndex(0);
        ui->queryResultTab->setCurrentIndex(0);
    }

    const auto milliseconds = static_cast<double>(time.elapsed());
    const auto msg = "Query execution took " + QString::number(milliseconds / 1000) + " seconds";
    this->showMessage(msg);

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
    const auto exporter = std::make_unique<DbSchemaExport>(info);
    const auto schema = exporter->exportSchema();
    ui->textEdit->setPlainText(schema);
}

void MainWindow::setEnabledActions(const bool enabled) {
    ui->actionRefresh->setEnabled(enabled);
    ui->actionShrink->setEnabled(enabled);
    ui->menuExport_Data->setEnabled(enabled);
    ui->actionScript_CSV->setEnabled(enabled);
    ui->actionScript_SQL->setEnabled(enabled);
    ui->actionExecute_Query->setEnabled(enabled);
    ui->actionScript_Schema->setEnabled(enabled);
    ui->actionCancel->setVisible(!enabled);
}

void MainWindow::exportDataToSqlScript() {
    const QString filepath = Prompts::getFilePath(this, QFileDialog::AcceptSave);
    if (filepath.isEmpty())
        return;

    DatabaseInfo info;
    analyzer->analyze(info);
    this->setEnabledActions(false);
    ui->queryResultTab->setCurrentIndex(1);

    exportOrchestrator->exportToSql(std::move(info), filepath, database.get());

    connect(exportOrchestrator.get(), &ExportOrchestrator::exportProgress,
            this, &MainWindow::onExportProgress);
    connect(exportOrchestrator.get(), &ExportOrchestrator::exportCompleted,
            this, &MainWindow::onExportCompleted);
}

void MainWindow::exportDataToCsvFiles() {
    const auto outputFolder = Prompts::getFolderPath(this);
    if (outputFolder.isEmpty()) {
        return;
    }

    sessionManager->setLastUsedExportPath(outputFolder);
    const auto delimiter = Prompts::getCsvDelimiter(this, ",");

    DatabaseInfo info;
    analyzer->analyze(info);
    this->setEnabledActions(false);
    ui->queryResultTab->setCurrentIndex(1);

    exportOrchestrator->exportToCsv(std::move(info), outputFolder, delimiter, database.get());

    connect(exportOrchestrator.get(), &ExportOrchestrator::exportProgress,
            this, &MainWindow::onExportProgress);
    connect(exportOrchestrator.get(), &ExportOrchestrator::exportCompleted,
            this, &MainWindow::onExportCompleted);
}

void MainWindow::cancel() const {
    exportOrchestrator->cancel();
}

void MainWindow::saveSql() {
    const auto sql = ui->textEdit->toPlainText();
    if (sql.isEmpty())
        return;
    const QString filepath = Prompts::getFilePath(this, QFileDialog::AcceptSave);
    const auto file = std::make_unique<QFile>(filepath);
    if (file->
        open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(file.get());
        out << sql;
        file->close();
    }
}

void MainWindow::treeNodeChanged(QTreeWidgetItem *item) const {
    treeNodeChanged(item, 0);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
// This is a slot method
void MainWindow::treeNodeChanged(QTreeWidgetItem *item,
                                 const int column) const {
    if (exportOrchestrator->isExporting()) {
        const auto msg = "Unable to process request. Data export in progress - " +
                         QString("%1 row(s)").arg(exportOrchestrator->getProgress()->getAffectedRows());
        this->showMessage(msg);
        ui->queryResultTab->setCurrentIndex(1);
        return;
    }
    if (item && item
        ->
        type() == QTreeWidgetItem::UserType + 1
    ) {
        if (!this->database->open()) {
            return;
        }

        const auto previousModel = dynamic_cast<QSqlTableModel *>(ui->tableView->model());
        if (previousModel != Q_NULLPTR) {
            previousModel->clear();
        }

        // ReSharper disable once CppDFAMemoryLeak
        const auto model = new QSqlTableModel(nullptr,
                                              this->database->getConnection());
        model->setTable(item->text(column));
        model->setEditStrategy(QSqlTableModel::OnFieldChange);

        ui->tableView->setModel(model);
        ui->tableView->setSortingEnabled(true);
        ui->tabWidget->setCurrentIndex(1);

        // ReSharper disable once CppDFAMemoryLeak
        delete previousModel;
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

void MainWindow::showMessage(const QString &message) const {
    ui->queryResultMessagesTextEdit->setPlainText(message);
    this->statusBar->showMessage(message, 5000);
}

void MainWindow::onExportProgress(uint64_t rowsExported) {
    showMessage(QString("Exported %1 row(s)").arg(rowsExported));
}

void MainWindow::onExportCompleted(uint64_t rowsExported) {
    setEnabledActions(true);
    showMessage(QString("Exported %1 row(s)").arg(rowsExported));
    ui->queryResultTab->setCurrentIndex(1);
}
