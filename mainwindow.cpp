#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    qDebug("MainWindow::MainWindow(QWidget*)");

    ui->setupUi(this);
    ui->splitterMain->setStretchFactor(0, 1);
    ui->splitterMain->setStretchFactor(1, 4);
    ui->splitterQueryTab->setStretchFactor(1, 1);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(createNewFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openExistingFile()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(appExit()));
    connect(ui->actionExecute_Query, SIGNAL(triggered()), this, SLOT(executeQuery()));
    connect(ui->actionShrink, SIGNAL(triggered()), this, SLOT(shrink()));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(treeNodeClicked(QTreeWidgetItem*,int)));

    database = new DatabaseAnalyzer();
    highlighter = new Highlighter(ui->textEdit->document());
}

MainWindow::~MainWindow()
{
    qDebug("MainWindow::~MainWindow()");

    delete database;
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

    if (!database->open(filename))
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
    creationDateNode->setText(0, QString("Creation date: ").append(info.creationDate.toLocalTime().toString()));

    QTreeWidgetItem *sizeNode = new QTreeWidgetItem(dbInfoNode);
    sizeNode->setText(0, QString("File size: ").append(getFileSize(info.size)));

    QTreeWidgetItem *tablesRootNode = new QTreeWidgetItem(ui->treeWidget);
    tablesRootNode->setText(0, "Tables");

    foreach (Table table, info.tables)
    {
        QTreeWidgetItem *tableNode = new QTreeWidgetItem(tablesRootNode, QTreeWidgetItem::UserType + 1);
        tableNode->setText(0, table.name);
    }

    ui->treeWidget->expandItem(dbInfoNode);
    ui->treeWidget->expandItem(tablesRootNode);
}

void MainWindow::openExistingFile()
{
    qDebug("MainWindow::openExistingFile()");

    QString filename = this->showFileDialog(QFileDialog::AcceptOpen);

    if (!database->open(filename))
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
    if (!database->analyze(info))
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

    if (!this->database->getDatabase().open())
    {
        qDebug("Unable to open database");
        return;
    }

    QStringList list(ui->textEdit->toPlainText().split(";", QString::SplitBehavior::SkipEmptyParts));
    QStringList errors;

    QSqlDatabase db = this->database->getDatabase();
    QSqlQuery query(db);

    foreach (QString sql, list)
    {
        QSqlError error;

        if (!query.exec(sql))
        {
            QString msg = (error = db.lastError()).isValid() ? error.text() : "Query execution failed";
            errors.append(msg);
            continue;
        }
    }

    if (errors.length() > 0)
    {
        analyzeDatabase();
        QString msg;
        foreach (QString error, errors)
        {
            msg += error;
            msg += "\r\n";
        }

        QMessageBox::information(this, "Error", msg, QMessageBox::Ok);
        return;
    }

    bool refreshTree = false;
    foreach (QString sql, list)
    {
        if (sql.contains("create", Qt::CaseInsensitive) || sql.contains("drop", Qt::CaseInsensitive) ||
            sql.contains("insert", Qt::CaseInsensitive) || sql.contains("delete", Qt::CaseInsensitive))
        {
            refreshTree = true;
            break;
        }
    }

    if (refreshTree)
        analyzeDatabase();
}

void MainWindow::treeNodeClicked(QTreeWidgetItem *item, int column)
{
    if (item && item->type() == QTreeWidgetItem::UserType + 1)
    {
        qDebug("table clicked");

        if (!this->database->getDatabase().open())
        {
            qDebug("Unable to open database");
            return;
        }

        QSqlTableModel *model = new QSqlTableModel(0, this->database->getDatabase());
        model->setTable(item->text(column));
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
        model->select();

        ui->tableView->setModel(model);
        ui->tabWidget->setCurrentIndex(1);
    }
}
