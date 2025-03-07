#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QStringList>
#include <QFile>
#include <QTextStream>

class RecentFiles
{
public:
    static void add(QString filePath);
    static void clear();
    static QStringList getList();

private:
    static QFile* openFile();
};

#endif // RECENTFILES_H
