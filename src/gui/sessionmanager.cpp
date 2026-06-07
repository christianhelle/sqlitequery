#include "sessionmanager.h"
#include "../settings/recentfiles.h"
#include "../settings/settings.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>

SessionManager::SessionManager(QObject *parent) : QObject(parent) {
}

void SessionManager::init() {
    Settings::init();
}

QString SessionManager::getSettingsFolder() {
    return Settings::getSettingsFolder();
}

void SessionManager::saveSession(const QString &sqliteFile, const QString &query) {
    Settings::setSessionState(sqliteFile, query);
}

void SessionManager::restoreSession(SessionState *state) const {
    Settings::getSessionState(state);
}

void SessionManager::saveWindowState(const WindowState &state) {
    Settings::setMainWindowState(
        state.size,
        state.position,
        state.treeWidth,
        state.tabWidth,
        state.queryTextHeight,
        state.queryResultHeight
    );
}

void SessionManager::restoreWindowState(WindowState *state) const {
    Settings::getMainWindowState(state);
}

void SessionManager::loadRecentFiles(QMenu *menu, QObject *parent) const {
    QStringList files = RecentFiles::getList();
    if (files.isEmpty())
        return;

    if (!menu->actions().isEmpty())
        menu->clear();

    foreach (const QString &file, files) {
        QAction *action = menu->addAction(file);
        action->setObjectName(file);
        connect(action, SIGNAL(triggered(bool)), parent, SLOT(openRecentFile()));
    }
}

void SessionManager::addRecentFile(const QString &filepath) const {
    RecentFiles::add(filepath);
}

QStringList SessionManager::getRecentFiles() const {
    return RecentFiles::getList();
}

void SessionManager::setLastUsedExportPath(const QString &path) {
    Settings::setLastUsedExportPath(path);
}

QString SessionManager::getLastUsedExportPath() const {
    SessionState state;
    Settings::getSessionState(&state);
    return state.lastUsedExportPath;
}
