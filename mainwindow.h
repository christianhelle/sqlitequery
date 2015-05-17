#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTreeWidget>

#include "dbanalyzer.h"
#include "highlighter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum NodeType : int
    {
        TableNode = QTreeWidgetItem::UserType + 1,
        ColumnNode = TableNode + 1,
        IndexNode = ColumnNode + 1
    };

public slots:
    void createNewFile();
    void openExistingFile();
    void appExit();
    void executeQuery();
    void treeNodeClicked(QTreeWidgetItem*,int);

private:
    QString showFileDialog(QFileDialog::AcceptMode mode);
    Ui::MainWindow *ui;

    DatabaseAnalyzer *database;
    Highlighter *highlighter;

    void populateTree(DatabaseInfo info);
    void analyzeDatabase();
};

#endif // MAINWINDOW_H
