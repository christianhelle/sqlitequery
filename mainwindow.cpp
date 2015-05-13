#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    qDebug("MainWindow::MainWindow(QWidget*)");

    ui->setupUi(this);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(createNewFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openExistingFile()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(appExit()));

    database = new DatabaseAnalyzer();
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

void MainWindow::openExistingFile()
{
    qDebug("MainWindow::openExistingFile()");

    QString filename = this->showFileDialog(QFileDialog::AcceptOpen);

    if (!database->open(filename))
    {
        qDebug("Unable to open file");
        return;
    }

    DatabaseInfo info;
    if (!database->analyze(info))
    {
        qDebug("Analyze database failed");
        return;
    }
}

void MainWindow::appExit()
{
    qDebug("MainWindow::appExit()");
    exit(0);
}
