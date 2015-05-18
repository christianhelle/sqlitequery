#ifndef DATABASEINFO
#define DATABASEINFO

#include "qglobal.h"

struct Column
{
    int ordinal;
    QString name;
    QString dataType;
    bool notNull;
    int maxLength;
};

struct Index
{
    QString name;
    QString column;
    bool unique;
    bool clustered;
};

struct Table
{
    QString name;
    QList<Column> columns;
    QList<Index> indexes;
};

struct DatabaseInfo
{
    QString filename;
    QDateTime creationDate;
    QString databaseVersion;
    bool passwordProtected;
    qint64 size;
    QList<Table> tables;
};

#endif // DATABASEINFO

