#ifndef SETTINGS_H
#define SETTINGS_H

#include <QPoint>
#include <QSizeF>

struct WindowState {
    QSize size;
    QPoint position;
    int treeWidth{};
    int tabWidth{};
    int queryTextHeight{};
    int queryResultHeight{};
};

struct SessionState {
    QString sqliteFile;
    QString query;
    QString lastUsedExportPath;
};

class Settings {
public:
    static void init();

    static QString getSettingsFolder();

    static void getMainWindowState(WindowState *state);

    static void setMainWindowState(const QSizeF &size,
                                   const QPoint &position,
                                   const int treeWidth,
                                   const int tabWidth,
                                   const int queryTextHeight,
                                   const int queryResultHeight);

    static void getSessionState(SessionState *state);

    static void setSessionState(const QString &sqliteFile,
                                const QString &query);

    static void setLastUsedExportPath(const QString &path);
};


#endif //SETTINGS_H
