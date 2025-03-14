#include "mainwindow.h"
#include "recentfiles.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
    qDebug("MainWindow::MainWindow(QWidget*)");

    ui->setupUi(this);
    ui->splitterMain->setStretchFactor(1, 3);
    ui->splitterQueryTab->setStretchFactor(1, 1);
    this->setWindowTitle("SQLite Query Analyzer");

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(createNewFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openExistingFile()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(appExit()));
    connect(ui->actionExecute_Query, SIGNAL(triggered()), this, SLOT(executeQuery()));
    connect(ui->actionShrink, SIGNAL(triggered()), this, SLOT(shrink()));
    connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this,
            SLOT(treeNodeChanged(QTreeWidgetItem*,int)));
    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*)), this,
            SLOT(treeNodeChanged(QTreeWidgetItem*)));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionRefresh, SIGNAL(triggered()), this, SLOT(refreshDatabase()));

    this->database = new Database();
    this->analyzer = new DbAnalyzer(database);
    this->query = new DbQuery(ui->queryResultsTableView, this->database);

    this->tree = new DbTree(ui->treeWidget);
    this->highlighter = new Highlighter(ui->textEdit->document());

    this->recentFilesMenu = new QMenu("Recent Files");
    ui->menuFile->insertMenu(ui->actionExit, recentFilesMenu);

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

void MainWindow::loadRecentFiles() {
    QStringList files = RecentFiles::getList();
    if (files.length() == 0)
        return;

    if (!this->recentFilesMenu->actions().empty())
        this->recentFilesMenu->clear();

    foreach(const QString file, files) {
        QAction *action = recentFilesMenu->addAction(file);
        action->setObjectName(file);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(openRecentFile()));
    }
}

void MainWindow::openRecentFile() {
    QString file = sender()->objectName();
    this->openDatabase(file);
}

QString MainWindow::showFileDialog(QFileDialog::AcceptMode mode) {
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
    qDebug("MainWindow::createNewFile()");

    QString filepath = this->showFileDialog(QFileDialog::AcceptSave);
    this->openDatabase(filepath);
    RecentFiles::add(filepath);
    this->loadRecentFiles();
}

void MainWindow::openDatabase(QString filename) {
    qDebug("MainWindow::openDatabase(QString)");

    this->database->setSource(filename);
    if (!this->database->open()) {
        qDebug("Unable to open file");
        return;
    }

    this->analyzeDatabase();
    RecentFiles::add(filename);
}

void MainWindow::openExistingFile() {
    qDebug("MainWindow::openExistingFile()");

    QString filepath = this->showFileDialog(QFileDialog::AcceptOpen);
    this->openDatabase(filepath);
    RecentFiles::add(filepath);
    this->loadRecentFiles();
}

void MainWindow::appExit() {
    qDebug("MainWindow::appExit()");
    exit(0);
}

void MainWindow::shrink() {
    QString filename = this->database->getFilename();
    if (filename.isNull() || filename.isEmpty())
        return;

    this->database->shrink();
    this->analyzeDatabase();
}

void MainWindow::refreshDatabase() {
    this->analyzeDatabase();
}

void MainWindow::analyzeDatabase() {
    qDebug("MainWindow::analyzeDatabase()");

    DatabaseInfo info;
    if (!analyzer->analyze(info)) {
        qDebug("Analyze database failed");
        return;
    }

    this->tree->populateTree(info);

    this->database->close();
}

void MainWindow::executeQuery() {
    qDebug("MainWindow::executeQuery()");

    QElapsedTimer time;
    time.start();

    QStringList errors;
    QString sql = ui->textEdit->toPlainText();
    if (this->query->execute(sql, &errors)) {
        ui->tabWidget->setCurrentIndex(0);
    }

    float milliseconds = (float) time.elapsed();
    QString msg = "Query execution took " + QString::number(milliseconds / 1000) + " seconds";
    ui->queryResultMessagesTextEdit->setPlainText(msg);

    if (sql.contains("create", Qt::CaseInsensitive) ||
        sql.contains("drop", Qt::CaseInsensitive) ||
        sql.contains("insert", Qt::CaseInsensitive) ||
        sql.contains("delete", Qt::CaseInsensitive)) {
        analyzeDatabase();
    }
}

void MainWindow::treeNodeChanged(QTreeWidgetItem *item) {
    treeNodeChanged(item, 0);
}

void MainWindow::treeNodeChanged(QTreeWidgetItem *item, int column) {
    if (item && item->type() == QTreeWidgetItem::UserType + 1) {
        qDebug("table selected");

        if (!this->database->open()) {
            qDebug("Unable to open database");
            return;
        }

        if (ui->tableView->model() != Q_NULLPTR)
            delete ui->tableView->model();

        QSqlTableModel *model = new QSqlTableModel(0, this->database->getDatabase());
        model->setTable(item->text(column));
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
        model->select();

        ui->tableView->setModel(model);
        ui->tableView->setSortingEnabled(true);
        ui->tabWidget->setCurrentIndex(1);
    }
}

void MainWindow::about() {
    QString text = "Database management and query analyzer tool for SQLite";
    QMessageBox::about(this, "About", text);
}
