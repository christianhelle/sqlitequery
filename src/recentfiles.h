#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <settings.h>

class RecentFiles {
public:
    static void add(const QString &filePath);

    static void clear();

    static QStringList getList();

private:
    static QFile *openFile();

    static QString getRecentsFilePath();
};

#endif // RECENTFILES_H
