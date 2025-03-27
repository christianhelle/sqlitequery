#ifndef DATABASEINFO
#define DATABASEINFO

#include <QtGlobal>
#include <QDateTime>
#include <qsqlquery.h>

struct Column {
    int ordinal;
    QString name;
    QString dataType;
    bool notNull;
    QString defaultValue;
    bool primaryKey;
};

struct Index {
    QString name;
    QString column;
    bool unique;
    bool clustered;
};

struct Table {
    QString name;
    QList<Column> columns;
    QList<Index> indexes;
};

struct DatabaseInfo {
    QString filename;
    QDateTime creationDate;
    QString databaseVersion;
    bool passwordProtected;
    qint64 size;
    QList<Table> tables;
};

#endif // DATABASEINFO
