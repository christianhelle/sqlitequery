#ifndef RECENTFILESADAPTER_H
#define RECENTFILESADAPTER_H

#include "irecentfiles.h"

class RecentFilesAdapter : public IRecentFiles {
public:
    void add(const QString &filepath) override;
    QStringList list() override;
    void clear() override;

private:
    static QString getRecentsFilePath();
    static QString sanitize(const QString &filepath);
};

#endif // RECENTFILESADAPTER_H
