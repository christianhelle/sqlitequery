#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>

struct QueryRow {
    QList<QVariant> values;
};

struct QueryResult {
    bool ok = true;
    QString error;
    bool isSelect = false;
    QStringList columns;
    QList<QueryRow> rows;
    int rowsAffected = 0;
};

#endif // QUERYRESULT_H
