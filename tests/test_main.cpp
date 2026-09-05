#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
    // Widget tests need a QApplication but no real display.
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "offscreen");

    // A MainWindow reads and writes the session, the window state and the
    // recent files list on the way up and down. Point every one of those at a
    // throwaway directory so running the tests cannot touch the settings of
    // whoever is running them.
    QTemporaryDir settingsDir;
    settingsDir.setAutoRemove(true);
    QStandardPaths::setTestModeEnabled(true);
    qputenv("HOME", settingsDir.path().toUtf8());
    qputenv("USERPROFILE", QDir::toNativeSeparators(settingsDir.path()).toUtf8());

    QApplication app(argc, argv);
    // Scopes QSettings, which would otherwise land in a shared default.
    QApplication::setOrganizationName("SQLiteQueryAnalyzerTests");
    QApplication::setApplicationName("SQLiteQueryAnalyzerTests");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
