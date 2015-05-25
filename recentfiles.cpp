#include "recentfiles.h"

RecentFiles::RecentFiles()
{
}

RecentFiles::~RecentFiles()
{
    this->clear();
}

void RecentFiles::add(QString filepath)
{
    this->files.append(filepath);
}

void RecentFiles::clear()
{
    this->files.clear();
}

QStringList RecentFiles::getList()
{
    return this->files;
}
