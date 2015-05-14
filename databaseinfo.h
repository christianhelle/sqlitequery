#ifndef DATABASEINFO
#define DATABASEINFO

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
    unsigned int size;
    QList<Table> tables;
};

#endif // DATABASEINFO

