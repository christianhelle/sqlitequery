#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <QString>
#include <QList>
#include <QVariantList>

struct QueryResult {
    bool success = false;
    struct Row {
        QList<QVariant> values;
    };
    QList<Row> rows;
    QStringList errors;
};

#endif // QUERYRESULT_H
