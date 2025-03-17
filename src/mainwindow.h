#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "dbanalyzer.h"
#include "dbquery.h"
#include "highlighter.h"
#include "dbtree.h"

namespace Ui {
    class MainWindow;
}

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void loadRecentFiles() const;

    void openDatabase(const QString &filename) const;

public slots:
    void createNewFile();

    void openExistingFile();

    static void appExit();

    void executeQuery() const;

    void treeNodeChanged(QTreeWidgetItem *, int) const;

    void treeNodeChanged(QTreeWidgetItem *) const;

    void shrink() const;

    void refreshDatabase() const;

    void about();

    void openRecentFile() const;

private:
    Ui::MainWindow *ui;
    QMenu *recentFilesMenu;
    Database *database;
    DbAnalyzer *analyzer;
    DbQuery *query;
    DbTree *tree;
    Highlighter *highlighter;

    QString showFileDialog(QFileDialog::AcceptMode mode);

    void analyzeDatabase() const;
};

#endif // MAINWINDOW_H
