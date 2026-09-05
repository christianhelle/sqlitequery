#include <gtest/gtest.h>
#include <QStandardPaths>
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
