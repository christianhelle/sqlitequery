#include "mainwindow.h"
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

    this->database = new Database();
    this->analyzer = new DbAnalyzer(database);
    this->query = new DbQuery(ui->splitterQueryTab, this->database);
    this->highlighter = new Highlighter(ui->textEdit->document());

    this->openExistingFile();
}

MainWindow::~MainWindow()
{
    qDebug("MainWindow::~MainWindow()");

    delete analyzer;
    delete ui;
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

    QString filename = this->showFileDialog(QFileDialog::AcceptSave);

    database->setSource(filename);
    if (!database->open())
    {
        qDebug("Unable to create file");
        return;
    }
}

QString getFileSize(qint64 size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024 && i.hasNext())
    {
        unit = i.next();
        num /= 1024.0;
    }

    return QString().setNum(num,'f',2) + " " + unit;
}

void MainWindow::populateTree(DatabaseInfo info)
{
    ui->treeWidget->clear();

    QTreeWidgetItem *dbInfoNode = new QTreeWidgetItem(ui->treeWidget);
    dbInfoNode->setText(0, "Database Info");

    QTreeWidgetItem *filenameNode = new QTreeWidgetItem(dbInfoNode);
    filenameNode->setText(0, QString("File name: ").append(info.filename));

    QTreeWidgetItem *creationDateNode = new QTreeWidgetItem(dbInfoNode);
    creationDateNode->setText(0, QString("Created on: ").append(info.creationDate.toLocalTime().toString()));

    QTreeWidgetItem *sizeNode = new QTreeWidgetItem(dbInfoNode);
    sizeNode->setText(0, QString("File size: ").append(getFileSize(info.size)));

    QTreeWidgetItem *tablesRootNode = new QTreeWidgetItem(ui->treeWidget);
    tablesRootNode->setText(0, "Tables");

    foreach (Table table, info.tables)
    {
        QTreeWidgetItem *tableNode = new QTreeWidgetItem(tablesRootNode, QTreeWidgetItem::UserType + 1);
        tableNode->setText(0, table.name);

        foreach (Column col, table.columns)
        {
            QTreeWidgetItem *colName = new QTreeWidgetItem(tableNode);
            colName->setText(0, col.name);

            QTreeWidgetItem *colOrdinal = new QTreeWidgetItem(colName);
            colOrdinal->setText(0, QString("Ordinal Position: ").append(QString::number(col.ordinal)));

            QTreeWidgetItem *colDataType = new QTreeWidgetItem(colName);
            colDataType->setText(0, QString("Data Type: ").append(col.dataType));

            QTreeWidgetItem *colNotNull = new QTreeWidgetItem(colName);
            colNotNull->setText(0, QString("Allow Null: ").append(!col.notNull ? "True" : "False"));
        }
    }

    ui->treeWidget->expandItem(dbInfoNode);
    ui->treeWidget->expandItem(tablesRootNode);
}

void MainWindow::openExistingFile()
{
    qDebug("MainWindow::openExistingFile()");

    QString filename = this->showFileDialog(QFileDialog::AcceptOpen);

    database->setSource(filename);
    if (!database->open())
    {
        qDebug("Unable to open file");
        return;
    }

    analyzeDatabase();
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

void MainWindow::analyzeDatabase()
{
    qDebug("MainWindow::analyzeDatabase()");

    DatabaseInfo info;
    if (!analyzer->analyze(info))
    {
        qDebug("Analyze database failed");
        return;
    }

    populateTree(info);

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
