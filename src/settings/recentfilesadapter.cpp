#include "recentfilesadapter.h"
#include "settings.h"

#include <QOperatingSystemVersion>
#include <QStringList>
#include <QFile>
#include <QTextStream>

QString RecentFilesAdapter::getRecentsFilePath() {
    return Settings::getSettingsFolder() + "/.recents";
}

QString RecentFilesAdapter::sanitize(const QString &filepath) {
    return QOperatingSystemVersion::currentType() != QOperatingSystemVersion::Windows
               ? QString(filepath).replace('\\', '/')
               : QString(filepath).replace('/', '\\');
}

void RecentFilesAdapter::add(const QString &filepath) {
    if (filepath.isEmpty() || !QFile::exists(filepath))
        return;

    auto files = list();
    const auto native_path = sanitize(filepath);
    if (files.contains(native_path, Qt::CaseInsensitive)) {
        return;
    }
    files.append(native_path);

    const QString recentsFilePath = RecentFilesAdapter::getRecentsFilePath();
    const auto file = std::make_unique<QFile>(recentsFilePath);
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
        return;
    }

    QTextStream out(file.get());
    for (const auto &path: files) {
        out << path << "\n";
    }

    file->close();
}

void RecentFilesAdapter::clear() {
    QFile(getRecentsFilePath()).deleteLater();
}

QStringList RecentFilesAdapter::list() {
    QStringList files;

    const QString filePath = RecentFilesAdapter::getRecentsFilePath();
    const auto file = std::make_unique<QFile>(filePath);
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
        return files;
    }

    if (QTextStream in(file.get()); in.seek(0)) {
        while (!in.atEnd()) {
            if (const auto path = sanitize(in.readLine());
                !files.contains(path, Qt::CaseInsensitive) && QFile::exists(path)) {
                files.append(path);
            }
        }
    }

    file->close();

    QStringList list;
    list.reserve(files.size());
    std::reverse_copy(files.begin(),
                      files.end(),
                      std::back_inserter(list));

    return list;
}
