#include "recentfiles.h"

#include <qstandardpaths.h>

QFile* RecentFiles::openFile()
{
    QString filePath = RecentFiles::getRecentsFilePath();
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug("Unable to open file");
        delete file;
        return Q_NULLPTR;
    }

    return file;
}

QString RecentFiles::getRecentsFilePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
        + "/recents";
}

void RecentFiles::add(const QString &filepath)
{
    if (filepath.isEmpty())
        return;

    QStringList files = RecentFiles::getList();
    if (files.contains(filepath, Qt::CaseInsensitive))
        return;

    QFile *file = openFile();
    if (file == Q_NULLPTR)
        return;

    if (file->seek(file->size()))
    {
        QTextStream out(file);
        out << filepath << "\n";
    }

    file->close();
    delete file;
}

void RecentFiles::clear()
{
    QFile(getRecentsFilePath()).deleteLater();
}

QStringList RecentFiles::getList()
{
    QStringList files;

    QFile *file = openFile();
    if (file == Q_NULLPTR)
        return files;

    QTextStream in(file);
    if (in.seek(0))
    {
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (!files.contains(line, Qt::CaseInsensitive))
                files.append(line);
        }
    }

    file->close();
    delete file;

    QStringList list;
    list.reserve(files.size());
    std::reverse_copy(files.begin(), files.end(), std::back_inserter(list));

    return list;
}
