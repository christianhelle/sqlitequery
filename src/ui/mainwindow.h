#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <qfuturewatcher.h>

#include "../threading/cancellation.h"
#include "../database/dbanalyzer.h"
#include "../database/dbexport.h"
#include "../database/dbexportdata.h"
#include "../database/dbquery.h"
#include "highlighter.h"
#include "../database/dbtree.h"

namespace Ui {
    class MainWindow;
}

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void connectSignalSlots() const;

    void resizeEvent(QResizeEvent *e) override;

    void loadRecentFiles() const;

    void openDatabase(const QString &filename);

    void restoreLastSession();

public slots:
    void createNewFile();

    void openExistingFile();

    [[noreturn]] void appExit() const;

    void executeQuery() const;

    void scriptSchema() const;

    void setEnabledActions(bool);

    void showExportDataProgress(const ExportDataProgress *progress,
                                CancellationToken cancellationToken) const;

    void exportDataAsync(const QString &filepath, const DatabaseInfo &info,
                         std::unique_ptr<ExportDataProgress>::pointer progress,
                         CancellationToken cancellationToken);

    void exportData();

    void cancel() const;

    void saveSql();

    void treeNodeChanged(QTreeWidgetItem *, int) const;

    void treeNodeChanged(QTreeWidgetItem *) const;

    void shrink() const;

    void refreshDatabase() const;

    void about();

    void openRecentFile();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QMenu> recentFilesMenu;
    std::unique_ptr<Database> database;
    std::unique_ptr<DbAnalyzer> analyzer;
    std::unique_ptr<DbQuery> query;
    std::unique_ptr<DbTree> tree;
    std::unique_ptr<Highlighter> highlighter;
    std::unique_ptr<ExportDataProgress> dataExportProgress;
    std::unique_ptr<CancellationTokenSource> tcs;
    bool loaded;

    QString showFileDialog(QFileDialog::AcceptMode mode);

    void analyzeDatabase() const;

    void saveSession() const;

    void saveWindowState(const QSize &size) const;

    void restoreWindowState();
};

#endif // MAINWINDOW_H
