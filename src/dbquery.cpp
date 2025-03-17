#include "dbquery.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QMessageBox>

DbQuery::DbQuery(QWidget *widget, Database *database)
{
    this->widget = widget;
    this->database = database;
    tableResults = new QList<QTableView*>();

    this->container = new QWidget(this->widget);
    this->scrollArea = new QScrollArea(this->widget);
}

void DbQuery::clearResults() const {
    this->container->setGeometry(this->widget->geometry());
    this->scrollArea->setGeometry(this->widget->geometry());
    this->scrollArea->setWidget(container);
    this->container->show();

    for (const auto &tableResult: *this->tableResults) {
        const QAbstractItemModel *model = tableResult->model();
        delete model;
    }

    qDeleteAll(this->tableResults->begin(), this->tableResults->end());
    this->tableResults->clear();
}

bool DbQuery::execute(const QStringList &queryList, QStringList *errors) const {
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

    const QSqlDatabase db = this->database->getDatabase();
    const QString empty;
    int count = 0;

    for (int i=0; i<queryList.length(); ++i)
    {
        const QString sql = queryList.at(i).trimmed().replace('\n', empty, Qt::CaseInsensitive);
        if (sql.isEmpty())
            continue;

        QSqlQuery query(db);
        if (!query.exec(sql))
        {
            QSqlError error;
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

            auto *model = new QSqlQueryModel();
            model->setQuery(sql);
            table->setModel(model);
            table->repaint();
        }
    }

    QRect newParentRect = this->widget->geometry();
    newParentRect.setHeight(newParentRect.height() * count);
    this->container->setGeometry(newParentRect);

    if (!errors->empty())
    {
        QString msg;
        for (const auto &error: *errors) {
            msg += error;
            msg += "\r\n";
        }

        QMessageBox::information(nullptr, "Error", msg, QMessageBox::Ok);
        return false;
    }

    return true;
}

