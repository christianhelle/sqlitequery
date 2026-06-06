#ifndef DBQUERY_H
#define DBQUERY_H

#include <QScrollArea>
#include <QTableView>

#include "idatabase.h"

class DbQuery {
public:
    DbQuery(QWidget *, IDatabase *);

    bool execute(const QStringList &, QStringList *);

    void clearResults();

private:
    QWidget *widget;
    IDatabase *database;

    std::unique_ptr<QScrollArea> scrollArea;
    std::unique_ptr<QWidget> container;
    QList<QTableView *> tableResults{};
};

#endif // DBQUERY_H
