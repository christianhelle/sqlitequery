#include "settings.h"

#include <QStandardPaths>
#include <QDir>
#include <QApplication>
#include <QSettings>

void Settings::init() {
    QApplication::setOrganizationDomain("christianhelle.com");
    QApplication::setOrganizationName("Christian Helle");
    QApplication::setApplicationName("SQLite Query Analyzer");

    const auto path = getSettingsFolder();
    if (const QDir dir(path); !dir.exists() && !dir.mkdir(path)) {
        qDebug("Failed to create settings directory");
    }

    const auto file = std::make_unique<QFile>(path + "/.settings.json");
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Unable to open settings file");
        return;
    }

    if (file->size() == 0) {
        file->write("{}");
    }

    file->close();
}

QString Settings::getSettingsFolder() {
    constexpr auto type = QStandardPaths::HomeLocation;
    const auto home_path = QStandardPaths::writableLocation(type);
    return home_path + "/.sqlite_query_analyzer";
}

void Settings::getMainWindowState(WindowState *state) {
    QSettings settings;
    settings.beginGroup("MainWindow");
    state->size = settings.value("main_window_size", QSize(800, 600)).toSize();
    state->position = settings.value("main_window_position", QPoint(0, 0)).toPoint();
    state->treeWidth = settings.value("main_window_tree_width", 0).toInt();
    state->tabWidth = settings.value("main_window_tab_width", 0).toInt();
    state->queryTextHeight = settings.value("main_window_query_text_height", 0).toInt();
    state->queryResultHeight = settings.value("main_window_query_result_height", 0).toInt();
    settings.endGroup();
}

void Settings::setMainWindowState(const QSizeF &size,
                                  const QPoint &position,
                                  const int treeWidth,
                                  const int tabWidth,
                                  const int queryTextHeight,
                                  const int queryResultHeight) {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("main_window_size", size);
    settings.setValue("main_window_position", position);
    if (treeWidth > 0 && tabWidth > 0) {
        settings.setValue("main_window_tree_width", treeWidth);
        settings.setValue("main_window_tab_width", tabWidth);
    }
    if (queryTextHeight > 0 && queryResultHeight > 0) {
        settings.setValue("main_window_query_text_height", queryTextHeight);
        settings.setValue("main_window_query_result_height", queryResultHeight);
    }
    settings.endGroup();
}

void Settings::getSessionState(SessionState *state) {
    QSettings settings;
    settings.beginGroup("Session");
    state->sqliteFile = settings.value("sqlite_file").toString();
    state->query = settings.value("query").toString();
    settings.endGroup();
}

void Settings::setSessionState(const QString &sqliteFile, const QString &query) {
    QSettings settings;
    settings.beginGroup("Session");
    settings.setValue("sqlite_file", sqliteFile);
    settings.setValue("query", query);
    settings.endGroup();
}
