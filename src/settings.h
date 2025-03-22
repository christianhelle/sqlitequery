#ifndef SETTINGS_H
#define SETTINGS_H

#include <QPoint>
#include <QSizeF>

struct WindowState {
    QSize size;
    QPoint position;
};

class Settings {
public:
    static void init();

    static QString getSettingsFolder();
    static void getMainWindowState(WindowState *state);
    static void setMainWindowState(const QSizeF &size, const QPoint &position);
};


#endif //SETTINGS_H
