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
#include "dbanalyzertask.h"

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

    Database* getDatabase() { return database; }
    void analyzeDatabase();

public slots:
    void createNewFile();
    void openExistingFile();
    void appExit();
    void executeQuery();
    void treeNodeChanged(QTreeWidgetItem*,int);
    void treeNodeChanged(QTreeWidgetItem*,QTreeWidgetItem*);
    void shrink();
    void refreshDatabase();
    void about();
    void openRecentFile();

private:
    Ui::MainWindow *ui;
    QMenu *recentFilesMenu;
    Database *database;
    DbTree *tree;
    Highlighter *highlighter;

    QString showFileDialog(QFileDialog::AcceptMode mode);
};

#endif // MAINWINDOW_H
