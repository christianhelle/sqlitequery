#include "dbquery.h"

DbQuery::DbQuery(QWidget *widget, Database *database)
{
    this->widget = widget;
    this->database = database;
    tableResults = new QList<QTableView*>();

    this->container = new QWidget(this->widget);
    this->scrollArea = new QScrollArea(this->widget);
    this->scrollArea->setGeometry(this->widget->geometry());
    this->scrollArea->setWidget(container);
}

void DbQuery::clearResults()
{
    this->container->setGeometry(this->widget->geometry());

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

    QRect widgetRect = this->widget->geometry();
    int yOffset = 0;
    const int width = widgetRect.width();
    const int height = widgetRect.height();

    QSqlDatabase db = this->database->getDatabase();
    const QString empty;
    int count = 0;

    for (int i=0; i<queryList.length(); ++i)
    {
        const QString sql = queryList.at(i).trimmed().replace('\n', empty, Qt::CaseInsensitive);
        if (sql == empty)
            continue;

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
            if (i > 0)
                yOffset += height;
            QRect rect = QRect(0, yOffset, width, height);
            QTableView *table = new QTableView(this->container);
            table->setGeometry(rect);
            table->show();

            this->tableResults->append(table);
            count++;

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(query);
            table->setModel(model);
        }
    }

    QRect newParentRect = this->widget->geometry();
    newParentRect.setHeight(newParentRect.height() * count);
    this->container->setGeometry(newParentRect);

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

