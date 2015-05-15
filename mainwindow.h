#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "dbanalyzer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void populateTree(DatabaseInfo info);
public slots:
    void createNewFile();
    void openExistingFile();
    void appExit();

private:
    QString showFileDialog(QFileDialog::AcceptMode mode);
    Ui::MainWindow *ui;

    DatabaseAnalyzer *database;
};

#endif // MAINWINDOW_H
