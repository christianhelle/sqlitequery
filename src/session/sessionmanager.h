#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "isessionmanager.h"
#include "../settings/isettings.h"

class SessionManager : public ISessionManager {
public:
    explicit SessionManager(ISettings *settings);

    void saveSession(const SessionData &session, const WindowData &window) override;
    SessionData loadSession() override;
    WindowData loadWindowData() override;
    void saveWindowData(const WindowData &window) override;

private:
    ISettings *settings;
};

#endif // SESSIONMANAGER_H
