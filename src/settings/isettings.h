#ifndef ISETTINGS_H
#define ISETTINGS_H

#include <QPoint>
#include <QSize>
#include <QString>

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

class ISettings {
public:
    virtual ~ISettings() = default;

    virtual void init() = 0;
    [[nodiscard]] virtual QString getSettingsFolder() = 0;
    virtual void getMainWindowState(WindowState *state) = 0;
    virtual void setMainWindowState(const QSize &size,
                                    const QPoint &position,
                                    int treeWidth,
                                    int tabWidth,
                                    int queryTextHeight,
                                    int queryResultHeight) = 0;
    virtual void getSessionState(SessionState *state) = 0;
    virtual void setSessionState(const QString &sqliteFile,
                                 const QString &query) = 0;
    virtual void setLastUsedExportPath(const QString &path) = 0;
};

#endif // ISETTINGS_H
