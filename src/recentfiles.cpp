#include "recentfiles.h"

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

void RecentFiles::add(const QString &filepath) {
    if (filepath.isEmpty())
        return;

    const QStringList files = RecentFiles::getList();
    if (files.contains(filepath, Qt::CaseInsensitive))
        return;

    QFile *file = openFile();
    if (file == Q_NULLPTR)
        return;

    if (file->seek(file->size())) {
        QTextStream out(file);
        out << filepath << "\n";
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
