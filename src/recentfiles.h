#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <settings.h>

class RecentFiles {
public:
    static void add(const QString &filepath);

    static void clear();

    static QStringList getList();

private:
    static QFile *openFile();

    static QString getRecentsFilePath();

    static QString sanitize(const QString &filepath);
};

#endif // RECENTFILES_H
