#include "dbquery.h"

DbQuery::DbQuery(QTableView *tableView, Database *database)
{
    this->tableView = tableView;
    this->database = database;
}

void DbQuery::clearResults()
{
    QAbstractItemModel *model = this->tableView->model();
    if (model != Q_NULLPTR)
        delete model;
}

bool DbQuery::execute(QString sql, QStringList *errors)
{
    this->clearResults();

    if (!this->database->open())
    {
        const QString msg = "Unable to open database";
        errors->append(msg);
        return false;
    }

    QSqlError error;
    QSqlDatabase db = this->database->getDatabase();
    QSqlQuery query(db);

    if (!query.exec(sql))
    {
        QString msg = (error = db.lastError()).isValid() ? error.text() : "Query execution failed";
        errors->append(msg);
    }

    if (query.isSelect())
    {
        QSqlQueryModel *model = new QSqlQueryModel();
        model->setQuery(sql);
        tableView->setModel(model);
    }

    if (errors->length() > 0)
    {
        QString msg;
        for (int i=0; i<errors->length(); ++i)
        {
            msg += errors->at(i);
            msg += "\r\n";
        }

        QMessageBox::information(0, "Error", msg, QMessageBox::Ok);
        return false;
    }

    return true;
}

