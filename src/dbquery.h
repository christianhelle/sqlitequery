#ifndef DBQUERY_H
#define DBQUERY_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QTableView>
#include <QMessageBox>

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
