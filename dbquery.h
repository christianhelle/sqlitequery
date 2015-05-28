#ifndef DBQUERY_H
#define DBQUERY_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QTableView>
#include <QMessageBox>

#include "database.h"

class DbQuery
{
public:
    DbQuery(QWidget*, Database*);
    bool execute(QStringList, QStringList*);

private:
    QWidget *widget;
    QList<QTableView*> *tableResults;
    Database *database;

    void clearResults();
};

#endif // DBQUERY_H
