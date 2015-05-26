#include "mainwindow.h"
#include "recentfiles.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
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
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(treeNodeClicked(QTreeWidgetItem*,int)));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionRefresh, SIGNAL(triggered()), this, SLOT(refreshDatabase()));

    this->database = new Database();
    this->analyzer = new DbAnalyzer(database);
    this->query = new DbQuery(ui->splitterQueryTab, this->database);
    this->tree = new DbTree(ui->treeWidget);
    this->highlighter = new Highlighter(ui->textEdit->document());

    this->loadRecentFiles();
    this->openExistingFile();
}

MainWindow::~MainWindow()
{
    qDebug("MainWindow::~MainWindow()");

    delete analyzer;
    delete ui;
}

void MainWindow::loadRecentFiles()
{
    QStringList files = RecentFiles::getList();
    if (files.length() == 0)
        return;

    QMenu *menu = ui->menuFile->addMenu("Recent Files");
    foreach (const QString file, files)
    {
        QAction *action = menu->addAction(file);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile(file)));
    }
}

void MainWindow::openRecentFile(const QString file)
{
    this->openDatabase(file);
}

QString MainWindow::showFileDialog(QFileDialog::AcceptMode mode)
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(mode);
    dialog.setDirectory(QDir::home());
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec())
    {
        QStringList files = dialog.selectedFiles();
        if (files.length())
            return files.first();
    }

    qDebug("File dialog cancelled");
    return Q_NULLPTR;
}

void MainWindow::createNewFile()
{
    qDebug("MainWindow::createNewFile()");

    QString filepath = this->showFileDialog(QFileDialog::AcceptSave);
    this->openDatabase(filepath);
    RecentFiles::add(filepath);
}

void MainWindow::openDatabase(QString filename)
{
    qDebug("MainWindow::openDatabase(QString)");

    this->database->setSource(filename);
    if (!this->database->open())
    {
        qDebug("Unable to open file");
        return;
    }

    this->analyzeDatabase();
}

void MainWindow::openExistingFile()
{
    qDebug("MainWindow::openExistingFile()");

    QString filepath = this->showFileDialog(QFileDialog::AcceptOpen);
    this->openDatabase(filepath);
    RecentFiles::add(filepath);
}

void MainWindow::appExit()
{
    qDebug("MainWindow::appExit()");
    exit(0);
}

void MainWindow::shrink()
{
    QString filename = this->database->getFilename();
    if (filename.isNull() || filename.isEmpty())
        return;

    this->database->shrink();
    this->analyzeDatabase();
}

void MainWindow::refreshDatabase()
{
    this->analyzeDatabase();
}

void MainWindow::analyzeDatabase()
{
    qDebug("MainWindow::analyzeDatabase()");

    DatabaseInfo info;
    if (!analyzer->analyze(info))
    {
        qDebug("Analyze database failed");
        return;
    }

    this->tree->populateTree(info);

    this->database->close();
}

void MainWindow::executeQuery()
{
    qDebug("MainWindow::executeQuery()");

    QStringList list(ui->textEdit->toPlainText().split(";", (QString::SplitBehavior) 1)); // SkipEmptyParts
    QStringList errors;

    if (this->query->execute(list, &errors))
    {
        ui->tabWidget->setCurrentIndex(0);
    }

    foreach (const QString sql, list)
    {
        if (sql.contains("create", Qt::CaseInsensitive) ||
            sql.contains("drop", Qt::CaseInsensitive) ||
            sql.contains("insert", Qt::CaseInsensitive) ||
            sql.contains("delete", Qt::CaseInsensitive))
        {
            analyzeDatabase();
            break;
        }
    }
}

void MainWindow::treeNodeClicked(QTreeWidgetItem *item, int column)
{
    if (item && item->type() == QTreeWidgetItem::UserType + 1)
    {
        qDebug("table clicked");

        if (!this->database->open())
        {
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
        ui->tabWidget->setCurrentIndex(1);
    }
}

void MainWindow::about()
{
    QString text = "Database management and query analyzer tool for SQLite";
    QMessageBox::about(this, "About", text);
}
