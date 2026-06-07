#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMenu>
#include <QSizeF>
#include <QPoint>
#include "../settings/settings.h"

class SessionManager : public QObject {
    Q_OBJECT

public:
    explicit SessionManager(QObject *parent = nullptr);

    static void init();

    // Session state
    void saveSession(const QString &sqliteFile, const QString &query);
    void restoreSession(SessionState *state) const;

    // Window state
    void saveWindowState(const WindowState &state);
    void restoreWindowState(WindowState *state) const;

    // Recent files
    void loadRecentFiles(QMenu *menu, QObject *parent) const;
    void addRecentFile(const QString &filepath) const;
    QStringList getRecentFiles() const;

    // Export path
    void setLastUsedExportPath(const QString &path);
    QString getLastUsedExportPath() const;

private:
    static QString getSettingsFolder();
};

#endif // SESSIONMANAGER_H
