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

    delete highlighter;
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

    const QString filepath = this->showFileDialog(QFileDialog::AcceptSave);
    this->openDatabase(filepath);
    RecentFiles::add(filepath);
    this->loadRecentFiles();
}

void MainWindow::openDatabase(const QString &filename) const {
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

void MainWindow::shrink() const {
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
    qDebug("MainWindow::analyzeDatabase()");

    DbAnalyzerTask *task = new DbAnalyzerTask(this->database, this->tree);
    QThreadPool::globalInstance()->start(task);
}

class DbQueryExecuteTask : public QRunnable
{
private:
    Ui_MainWindow *ui;
    const MainWindow *mainWindow;
    QStringList list;

public:
    DbQueryExecuteTask(Ui_MainWindow *ui, const MainWindow *instance)
    {
        this->ui = ui;
        this->mainWindow = instance;
        list = QStringList(ui->textEdit->toPlainText().split(";", Qt::SkipEmptyParts));
    }

    void run() override
    {
        emit ui->queryResultMessagesTextEdit->setPlainText("Executing query...");

        QElapsedTimer time;
        time.start();

        QStringList errors;
        auto *query = new DbQuery(ui->queryResultsGrid, this->mainWindow->getDatabase());
        if (query->execute(list, &errors))
        {
            emit ui->tabWidget->setCurrentIndex(0);
	        emit ui->queryResultTab->setCurrentIndex(0);
        }
        
    	const auto milliseconds = static_cast<double>(time.elapsed());
    	const auto msg = "Query execution took " + QString::number(milliseconds / 1000) + " seconds";
        emit ui->queryResultMessagesTextEdit->setPlainText(msg);

        foreach (const QString sql, list)
        {
            if (sql.contains("create", Qt::CaseInsensitive) ||
                sql.contains("drop", Qt::CaseInsensitive) ||
                sql.contains("insert", Qt::CaseInsensitive) ||
                sql.contains("delete", Qt::CaseInsensitive))
            {
	            emit ui->queryResultTab->setCurrentIndex(1);
                mainWindow->analyzeDatabase();
                break;
            }
        }

        delete query;
    }
};

void MainWindow::executeQuery() const
{
    qDebug("MainWindow::executeQuery()");

    DbQueryExecuteTask *task = new DbQueryExecuteTask(this->ui, this);
    QThreadPool::globalInstance()->start(task);
}

void MainWindow::treeNodeChanged(const QTreeWidgetItem *item) const {
    treeNodeChanged(item, 0);
}

void MainWindow::treeNodeChanged(const QTreeWidgetItem *item, const int column) const {
    if (item && item->type() == QTreeWidgetItem::UserType + 1) {
        qDebug("table selected");

        if (!this->database->open()) {
            qDebug("Unable to open database");
            return;
        }

        if (ui->tableView->model() != Q_NULLPTR)
            delete ui->tableView->model();

        auto *model = new QSqlTableModel(nullptr, this->database->getDatabase());
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
