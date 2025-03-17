#include "settings.h"

#include <QStandardPaths>
#include <QDir>

void Settings::init() {
    const auto path = getSettingsFolder();
    if (const QDir dir(path); !dir.exists() && !dir.mkdir(path)) {
        qDebug("Failed to create settings directory");
    }

    const auto file = std::make_unique<QFile>(path + "/.settings.json");
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Unable to open settings file");
        return;
    }

    if (file->size() == 0) {
        file->write("{}");
    }

    file->close();
}

QString Settings::getSettingsFolder() {
    constexpr auto type = QStandardPaths::HomeLocation;
    const auto home_path = QStandardPaths::writableLocation(type);
    return home_path + "/.sqlite_query_analyzer";
}
