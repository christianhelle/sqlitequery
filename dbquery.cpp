#include "dbquery.h"

DbQuery::DbQuery(QWidget *widget, Database *database)
{
    this->widget = widget;
    this->database = database;
    tableResults = new QList<QTableView*>();
}

void DbQuery::clearResults()
{
    QList<QTableView*>::iterator i;
    for (i = this->tableResults->begin(); i != this->tableResults->end(); ++i)
    {
        QAbstractItemModel *model = (*i)->model();
        if (model != Q_NULLPTR)
            delete model;
    }

    qDeleteAll(this->tableResults->begin(), this->tableResults->end());
    this->tableResults->clear();
}

bool DbQuery::execute(QStringList queryList, QStringList *errors)
{
    this->clearResults();

    if (!this->database->open())
    {
        const QString msg = "Unable to open database";
        errors->append(msg);
        return false;
    }

    QSqlDatabase db = this->database->getDatabase();

    for (int i=0; i<queryList.length(); ++i)
    {
        const QString sql = queryList.at(i);
        QSqlError error;
        QSqlQuery query(db);

        if (!query.exec(sql))
        {
            QString msg = (error = db.lastError()).isValid() ? error.text() : "Query execution failed";
            errors->append(msg);
            continue;
        }

        if (query.isSelect())
        {
            QRect rect = this->widget->geometry();
            rect.setY(rect.y() * i+1);

            QTableView *table = new QTableView(this->widget);
            table->setGeometry(rect);
            table->show();

            this->tableResults->append(table);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(query);
            table->setModel(model);
        }
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

