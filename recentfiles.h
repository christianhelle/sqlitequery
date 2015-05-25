#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QStringList>

class RecentFiles
{
public:
    RecentFiles();
    ~RecentFiles();

    void add(QString filePath);
    void clear();
    QStringList getList();

private:
    QStringList files;
};

#endif // RECENTFILES_H
