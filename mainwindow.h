#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTreeWidget>
#include <QTableView>

#include "dbanalyzer.h"
#include "dbquery.h"
#include "highlighter.h"
#include "dbtree.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadRecentFiles();
    void openDatabase(QString filename);
public slots:
    void createNewFile();
    void openExistingFile();
    void appExit();
    void executeQuery();
    void treeNodeClicked(QTreeWidgetItem*,int);
    void shrink();
    void refreshDatabase();
    void about();
    void openRecentFile(const QString file);

private:
    QString showFileDialog(QFileDialog::AcceptMode mode);
    Ui::MainWindow *ui;

    Database *database;
    DbAnalyzer *analyzer;
    DbQuery *query;
    DbTree *tree;
    Highlighter *highlighter;

    void analyzeDatabase();
};

#endif // MAINWINDOW_H
