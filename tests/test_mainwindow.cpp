#include <gtest/gtest.h>
#include <QStandardPaths>
#include <QAction>
#include <QFontInfo>
#include <QPlainTextEdit>
#include <QTableView>
#include <QTextEdit>
#include <QTreeWidget>
#include <memory>

#include "database/inmemorydatabase.h"
#include "database/queryexecutor.h"
#include "gui/mainwindow.h"

// The window takes an IDatabase, so it can be driven against the in-memory
// adapter. Before that it built its own SqliteDatabase and none of this was
// reachable from a test.
class MainWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Guards the developer's own settings: the window reads and writes the
        // session and recent files as it is built and torn down.
        ASSERT_TRUE(QStandardPaths::isTestModeEnabled());

        db = std::make_unique<InMemoryDatabase>();
        db->setSource(":memory:");
        db->open();

        QStringList sql;
        sql << "CREATE TABLE inserted_at (id INTEGER PRIMARY KEY, name TEXT)";
        QueryExecutor(db.get()).runStatements(sql);
    }

    [[nodiscard]] static QStringList tableNames(const MainWindow &window) {
        QStringList names;
        const auto *tree = window.findChild<QTreeWidget *>();
        if (tree == nullptr)
            return names;
        for (int i = 0; i < tree->topLevelItemCount(); ++i) {
            const auto *top = tree->topLevelItem(i);
            if (top->text(0) != "Tables")
                continue;
            for (int c = 0; c < top->childCount(); ++c)
                names << top->child(c)->text(0);
        }
        return names;
    }

    // The query editor, reached the way any Qt test reaches a widget in a
    // window rather than by widening the window's own interface for a test.
    static void typeQuery(const MainWindow &window, const QString &sql) {
        auto *editor = window.findChild<QTextEdit *>("textEdit");
        ASSERT_NE(editor, nullptr);
        editor->setPlainText(sql);
    }

    // Zoom is driven the way the user drives it, through the menu actions,
    // rather than by reaching for the presenters the window keeps private.
    static void zoomIn(const MainWindow &window) {
        auto *action = window.findChild<QAction *>("actionZoom_In");
        ASSERT_NE(action, nullptr);
        action->trigger();
    }

    static void resetZoom(const MainWindow &window) {
        auto *action = window.findChild<QAction *>("actionReset_Zoom");
        ASSERT_NE(action, nullptr);
        action->trigger();
    }

    // The views a run rendered into, which live under the results grid. The
    // Table Data tab holds a QTableView of its own, so the search starts at
    // the grid rather than at the window.
    static QList<QTableView *> resultViews(const MainWindow &window) {
        auto *grid = window.findChild<QWidget *>("queryResultsGrid");
        if (grid == nullptr)
            return {};
        return grid->findChildren<QTableView *>();
    }

    // The measure ZoomPresenter works in. A widget that inherits its font, or
    // that was sized in pixels, reports no point size of its own, so reading
    // pointSizeF() straight off it can yield -1 and turn a correct scaling
    // into a failed comparison.
    static double pointSize(const QWidget *widget) {
        const double size = widget->font().pointSizeF();
        return size > 0 ? size : QFontInfo(widget->font()).pointSizeF();
    }

    std::unique_ptr<InMemoryDatabase> db;
};

TEST_F(MainWindowTest, IsBuiltAgainstTheDatabaseItIsGiven) {
    const MainWindow window(db.get());

    EXPECT_NE(window.findChild<QTreeWidget *>(), nullptr);
}

TEST_F(MainWindowTest, RefreshingShowsTheTablesOfTheInjectedDatabase) {
    MainWindow window(db.get());

    window.refreshDatabase();

    EXPECT_TRUE(tableNames(window).contains("inserted_at"));
}

// The whole chain: a select naming a DDL word used to re-analyse the database
// on every run. It should leave the tree alone.
TEST_F(MainWindowTest, ASelectNamingADdlWordDoesNotDisturbTheTree) {
    MainWindow window(db.get());
    window.refreshDatabase();
    const QStringList before = tableNames(window);
    ASSERT_FALSE(before.isEmpty());

    QStringList sql;
    sql << "CREATE TABLE added_behind_the_window (id INTEGER)";
    QueryExecutor(db.get()).runStatements(sql);

    typeQuery(window, "SELECT * FROM inserted_at");
    window.executeQuery();

    // Still the old list: the select changed no schema, so nothing re-read it.
    EXPECT_EQ(tableNames(window), before);
}

TEST_F(MainWindowTest, ACreateRefreshesTheTree) {
    MainWindow window(db.get());
    window.refreshDatabase();
    ASSERT_FALSE(tableNames(window).contains("made_by_the_window"));

    typeQuery(window, "CREATE TABLE made_by_the_window (id INTEGER)");
    window.executeQuery();

    EXPECT_TRUE(tableNames(window).contains("made_by_the_window"));
}

// The messages pane rides on the editor's zoom, so one zoom moves both and
// they stay proportional to the sizes they were built with.
TEST_F(MainWindowTest, ZoomingTheEditorZoomsTheResultMessagesPane) {
    const MainWindow window(db.get());
    auto *editor = window.findChild<QTextEdit *>("textEdit");
    auto *messages = window.findChild<QPlainTextEdit *>("queryResultMessagesTextEdit");
    ASSERT_NE(editor, nullptr);
    ASSERT_NE(messages, nullptr);

    // The window restores whatever zoom the last run left behind, so start
    // from a known step rather than from that.
    resetZoom(window);
    const double editorBefore = pointSize(editor);
    const double messagesBefore = pointSize(messages);

    zoomIn(window);

    EXPECT_GT(pointSize(editor), editorBefore);
    EXPECT_NEAR(pointSize(messages),
                messagesBefore * (pointSize(editor) / editorBefore), 0.001);

    // The window persists its zoom step as it is torn down, so hand the next
    // test the same starting point this one was given.
    resetZoom(window);
}

// Every run builds its result views from scratch, so they have to come up at
// the step the editor is already at rather than at their own built size.
TEST_F(MainWindowTest, ResultViewsComeUpAtTheEditorZoom) {
    const MainWindow window(db.get());
    resetZoom(window);

    typeQuery(window, "SELECT 1");
    window.executeQuery();
    ASSERT_FALSE(resultViews(window).isEmpty());
    const double base = resultViews(window).first()->font().pointSizeF();

    // Zoom, then run again. The second run's views are new widgets, and the
    // ones measured above are gone by the time they are registered.
    zoomIn(window);
    window.executeQuery();

    ASSERT_FALSE(resultViews(window).isEmpty());
    EXPECT_NEAR(resultViews(window).first()->font().pointSizeF(), base * 1.1, 0.001);
}
