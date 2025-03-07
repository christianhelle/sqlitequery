#include "recentfiles.h"

QFile* RecentFiles::openFile()
{
    QFile *file = new QFile("recents");
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug("Unable to open file");
        delete file;
        return Q_NULLPTR;
    }

    return file;
}

void RecentFiles::add(QString filepath)
{
    if (filepath == "")
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
    QFile("recents").deleteLater();
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
