#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStandardPaths>
#include <QString>
#include <QDir>

class Settings {
public:
    static void init();

    static QString getSettingsFolder();
};



#endif //SETTINGS_H
