#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings {
public:
    static void init();

    static QString getSettingsFolder();
};


#endif //SETTINGS_H
