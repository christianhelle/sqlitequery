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
#include <QStackedWidget>

#include "database.h"

class DbQuery
{
public:
    DbQuery(QTableView*, Database*);
    bool execute(QString, QStringList*);

private:
    QTableView *tableView;
    Database *database;

    void clearResults();
};

#endif // DBQUERY_H
