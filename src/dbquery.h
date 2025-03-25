#ifndef DBQUERY_H
#define DBQUERY_H

#include <QScrollArea>
#include <QTableView>

#include "database.h"

class DbQuery {
public:
    DbQuery(QWidget *, Database *);

    bool execute(const QStringList &, QStringList *);

    void clearResults();

private:
    QWidget *widget;
    Database *database;

    std::unique_ptr<QScrollArea> scrollArea;
    std::unique_ptr<QWidget> container;
    QList<QTableView *> tableResults;
};

#endif // DBQUERY_H
