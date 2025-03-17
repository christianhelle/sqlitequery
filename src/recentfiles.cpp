#include "recentfiles.h"
#include "settings.h"

#include <QOperatingSystemVersion>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>

QFile *RecentFiles::openFile() {
    const QString filePath = RecentFiles::getRecentsFilePath();
    const auto file = new QFile(filePath);
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Unable to open file");
        delete file;
        return Q_NULLPTR;
    }

    return file;
}

QString RecentFiles::getRecentsFilePath() {
    return Settings::getSettingsFolder() + "/.recents";
}

QString RecentFiles::sanitize(const QString &filepath) {
    return QOperatingSystemVersion::currentType() != QOperatingSystemVersion::Windows
               ? QString(filepath).replace('\\', '/')
               : QString(filepath).replace('/', '\\');
}

void RecentFiles::add(const QString &filepath) {
    if (filepath.isEmpty())
        return;

    const auto native_path = sanitize(filepath);
    if (getList().contains(native_path, Qt::CaseInsensitive))
        return;

    QFile *file = openFile();
    if (file == Q_NULLPTR)
        return;

    if (file->seek(file->size())) {
        QTextStream out(file);
        out << native_path << "\n";
    }

    file->close();
    delete file;
}

void RecentFiles::clear() {
    QFile(getRecentsFilePath()).deleteLater();
}

QStringList RecentFiles::getList() {
    QStringList files;

    QFile *file = openFile();
    if (file == Q_NULLPTR)
        return files;

    if (QTextStream in(file); in.seek(0)) {
        while (!in.atEnd()) {
            if (QString line = in.readLine(); !files.contains(line, Qt::CaseInsensitive))
                files.append(line);
        }
    }

    file->close();
    delete file;

    QStringList list;
    list.reserve(files.size());
    std::reverse_copy(files.begin(),
                      files.end(),
                      std::back_inserter(list));

    return list;
}
