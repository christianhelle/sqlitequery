#ifndef ISESSIONMANAGER_H
#define ISESSIONMANAGER_H

#include <QString>
#include <QSize>
#include <QPoint>

struct SessionData {
    QString sqliteFile;
    QString queryText;
};

struct WindowData {
    QSize size;
    QPoint position;
    int treeWidth;
    int tabWidth;
    int queryTextHeight;
    int queryResultHeight;
};

class ISessionManager {
public:
    virtual ~ISessionManager() = default;

    virtual void saveSession(const SessionData &session, const WindowData &window) = 0;
    [[nodiscard]] virtual SessionData loadSession() = 0;
    [[nodiscard]] virtual WindowData loadWindowData() = 0;
    virtual void saveWindowData(const WindowData &window) = 0;
};

#endif // ISESSIONMANAGER_H
