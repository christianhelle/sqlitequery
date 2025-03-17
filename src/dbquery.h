#ifndef DBQUERY_H
#define DBQUERY_H

#include <QScrollArea>
#include <QTableView>

#include "database.h"

class DbQuery {
public:
    DbQuery(QWidget *, Database *);

    bool execute(const QStringList &, QStringList *) const;

private:
    QWidget *widget;
    QScrollArea *scrollArea;
    QWidget *container;
    QList<QTableView *> *tableResults;
    Database *database;

    void clearResults() const;
};

#endif // DBQUERY_H
