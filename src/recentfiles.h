#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QFile>

class RecentFiles {
public:
    static void add(const QString &filepath);

    static void clear();

    static QStringList getList();

private:
    static QString getRecentsFilePath();

    static QString sanitize(const QString &filepath);
};

#endif // RECENTFILES_H
