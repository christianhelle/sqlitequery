#ifndef SETTINGSADAPTER_H
#define SETTINGSADAPTER_H

#include "isettings.h"

class SettingsAdapter : public ISettings {
public:
    void init() override;
    QString getSettingsFolder() override;
    void getMainWindowState(WindowState *state) override;
    void setMainWindowState(const QSize &size,
                            const QPoint &position,
                            int treeWidth,
                            int tabWidth,
                            int queryTextHeight,
                            int queryResultHeight) override;
    void getSessionState(SessionState *state) override;
    void setSessionState(const QString &sqliteFile,
                         const QString &query) override;
    void setLastUsedExportPath(const QString &path) override;
};

#endif // SETTINGSADAPTER_H
