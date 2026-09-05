#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>

#include "../threading/cancellation.h"
#include "../database/dbanalyzer.h"
#include "../database/dbexport.h"
#include "../database/dbexportdata.h"
#include "../database/dbtree.h"
#include "../database/queryexecutor.h"
#include "../database/sqlitedatabase.h"
#include "highlighter.h"
#include "queryexecutionpresenter.h"
#include "sessionmanager.h"
#include "zoompresenter.h"
#include "exportorchestrator.h"

namespace Ui {
    class MainWindow;
}

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void deleteSelectedTable();

    void keyPressEvent(QKeyEvent *event) override;

    void connectSignalSlots() const;

    void resizeEvent(QResizeEvent *e) override;

    void openDatabase(const QString &filename);

    void restoreLastSession();

public slots:
    void createNewFile();

    void openExistingFile();

    [[noreturn]] void appExit() const;

    void executeQuery() const;

    void scriptSchema() const;

    void setEnabledActions(bool);

    void exportDataToSqlScript();

    void exportDataToCsvFiles();

    void cancel() const;

    void saveSql();

    void treeNodeChanged(QTreeWidgetItem *, int) const;

    void treeNodeChanged(QTreeWidgetItem *) const;

    void shrink() const;

    void refreshDatabase() const;

    void about();

    void openRecentFile();

    void onExportProgress(uint64_t rowsExported);

    void onExportCompleted(uint64_t rowsExported);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QMenu> recentFilesMenu;
    std::unique_ptr<SqliteDatabase> database;
    std::unique_ptr<DbAnalyzer> analyzer;
    std::unique_ptr<QueryExecutor> executor;
    std::unique_ptr<QueryExecutionPresenter> queryPresenter;
    std::unique_ptr<DbTree> tree;
    std::unique_ptr<Highlighter> highlighter;
    std::unique_ptr<SessionManager> sessionManager;
    std::unique_ptr<ExportOrchestrator> exportOrchestrator;
    std::unique_ptr<ZoomPresenter> editorZoom;
    std::unique_ptr<ZoomPresenter> treeZoom;
    bool loaded = false;

    // The pane a keyboard zoom applies to: whichever of the two holds the
    // focus, falling back to the editor when the focus is elsewhere.
    [[nodiscard]] ZoomPresenter *zoomForFocus() const;

    void analyzeDatabase() const;

    void saveSession() const;

    void saveWindowState(const QSize &size) const;

    void restoreWindowState();

    void showMessage(const QString &message) const;

    // Reports that an export is in progress and returns true when it is, so
    // callers can bail out with a single guard.
    [[nodiscard]] bool blockedByExport() const;
};

#endif // MAINWINDOW_H
