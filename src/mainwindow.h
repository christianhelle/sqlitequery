#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <qfuturewatcher.h>

#include "cancellation.h"
#include "dbanalyzer.h"
#include "dbexport.h"
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

    void scriptSchema() const;

    void setEnabledActions(bool);
    void showExportDataProgress(std::unique_ptr<ExportDataProgress>::pointer progress,
                                CancellationToken cancellationToken) const;
    void scriptDataAsync(const QString& filepath, const DatabaseInfo& info, std::unique_ptr<ExportDataProgress>::pointer progress,
                         CancellationToken cancellationToken);

    void scriptData();

    void cancel() const;

    void saveSql();

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
    std::unique_ptr<ExportDataProgress> dataExportProgress;
    std::unique_ptr<CancellationTokenSource> tcs;

    QString showFileDialog(QFileDialog::AcceptMode mode);

    void analyzeDatabase() const;
};

#endif // MAINWINDOW_H
