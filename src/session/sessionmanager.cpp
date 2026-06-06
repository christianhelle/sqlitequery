#include "sessionmanager.h"

SessionManager::SessionManager(ISettings *settings)
    : settings(settings) {
}

void SessionManager::saveSession(const SessionData &session, const WindowData &window) {
    SessionState state;
    state.sqliteFile = session.sqliteFile;
    state.query = session.queryText;
    settings->setSessionState(state.sqliteFile, state.query);

    WindowData currentWindow = window;
    settings->setMainWindowState(
        currentWindow.size,
        currentWindow.position,
        currentWindow.treeWidth,
        currentWindow.tabWidth,
        currentWindow.queryTextHeight,
        currentWindow.queryResultHeight
    );
}

SessionData SessionManager::loadSession() {
    SessionState state;
    settings->getSessionState(&state);

    SessionData session;
    session.sqliteFile = state.sqliteFile;
    session.queryText = state.query;
    return session;
}

WindowData SessionManager::loadWindowData() {
    WindowState state;
    settings->getMainWindowState(&state);

    WindowData window;
    window.size = state.size;
    window.position = state.position;
    window.treeWidth = state.treeWidth;
    window.tabWidth = state.tabWidth;
    window.queryTextHeight = state.queryTextHeight;
    window.queryResultHeight = state.queryResultHeight;
    return window;
}

void SessionManager::saveWindowData(const WindowData &window) {
    settings->setMainWindowState(
        window.size,
        window.position,
        window.treeWidth,
        window.tabWidth,
        window.queryTextHeight,
        window.queryResultHeight
    );
}
