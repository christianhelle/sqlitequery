#include "settings.h"

#include <QStandardPaths>
#include <QDir>
#include <QApplication>
#include <QSettings>

void Settings::init() {
    const auto path = getSettingsFolder();
    if (const QDir dir(path); !dir.exists() && !dir.mkdir(path)) {
        // Failed to create settings directory
    }

    const auto file = std::make_unique<QFile>(path + "/.settings.json");
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
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
    state->editorZoomStep = settings.value("main_window_editor_zoom_step", 0).toInt();
    state->treeZoomStep = settings.value("main_window_tree_zoom_step", 0).toInt();
    settings.endGroup();
}

void Settings::setMainWindowState(const WindowState &state) {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("main_window_size", state.size);
    settings.setValue("main_window_position", state.position);
    if (state.treeWidth > 0 && state.tabWidth > 0) {
        settings.setValue("main_window_tree_width", state.treeWidth);
        settings.setValue("main_window_tab_width", state.tabWidth);
    }
    if (state.queryTextHeight > 0 && state.queryResultHeight > 0) {
        settings.setValue("main_window_query_text_height", state.queryTextHeight);
        settings.setValue("main_window_query_result_height", state.queryResultHeight);
    }
    settings.setValue("main_window_editor_zoom_step", state.editorZoomStep);
    settings.setValue("main_window_tree_zoom_step", state.treeZoomStep);
    settings.endGroup();
}

void Settings::getSessionState(SessionState *state) {
    QSettings settings;
    settings.beginGroup("Session");
    state->sqliteFile = settings.value("sqlite_file").toString();
    state->query = settings.value("query").toString();
    state->lastUsedExportPath = settings.value("last_used_export_path").toString();
    settings.endGroup();
}

void Settings::setSessionState(const QString &sqliteFile,
                               const QString &query) {
    QSettings settings;
    settings.beginGroup("Session");
    settings.setValue("sqlite_file", sqliteFile);
    settings.setValue("query", query);
    settings.endGroup();
}

void Settings::setLastUsedExportPath(const QString &path) {
    QSettings settings;
    settings.beginGroup("Session");
    settings.setValue("last_used_export_path", path);
    settings.endGroup();
}
