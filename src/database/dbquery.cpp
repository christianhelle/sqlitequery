#include "dbquery.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QMessageBox>

DbQuery::DbQuery(QWidget *widget, Database *database)
    : widget(widget),
      database(database),
      tableResults(QList<QTableView *>()) {
    this->container = std::make_unique<QWidget>(this->widget);
    this->container->hide();
    this->scrollArea = std::make_unique<QScrollArea>(this->widget);
    this->scrollArea->setWidget(container.get());
    this->scrollArea->hide();
}

void DbQuery::clearResults() {
    this->container->setGeometry(this->widget->geometry());
    this->container->show();

    this->scrollArea->setGeometry(this->widget->geometry());
    this->scrollArea->show();

    for (const auto table: this->tableResults) {
        const auto model = dynamic_cast<QSqlQueryModel *>(table->model());
        if (model != Q_NULLPTR) {
            model->clear();
        }
        table->setModel(Q_NULLPTR);
        delete model;
    }
    qDeleteAll(this->tableResults.begin(), this->tableResults.end());
    this->tableResults.clear();
}

bool DbQuery::execute(const QStringList &queryList, QStringList *errors) {
    this->clearResults();

    if (!this->database->open()) {
        const QString msg = "Unable to open database";
        errors->append(msg);
        return false;
    }

    QRect widgetRect = this->widget->geometry();
    int yOffset = 0;
    const int width = widgetRect.width();
    const int height = widgetRect.height();

    const QSqlDatabase db = this->database->getDatabase();
    int count = 0;

    for (int i = 0; i < queryList.length(); ++i) {
        const QString sql = queryList.at(i).trimmed().replace('\n', "", Qt::CaseInsensitive);
        if (sql.isEmpty())
            continue;

        QSqlQuery query(db);
        if (!query.exec(sql)) {
            QSqlError error;
            QString msg = (error = db.lastError()).isValid() ? error.text() : "Query execution failed";
            errors->append(msg);
            continue;
        }

        // ReSharper disable CppDFAMemoryLeak
        if (query.isSelect()) {
            if (i > 0)
                yOffset += height;
            auto rect = QRect(0, yOffset, width, height);
            const auto ptr = this->container.get();
            auto *table = new QTableView(ptr);
            table->setGeometry(rect);
            table->show();

            this->tableResults.append(table);
            count++;

            auto *model = new QSqlQueryModel();
            model->setQuery(std::move(query));
            table->setModel(model);
            table->repaint();
        }
    }

    QRect newParentRect = this->widget->geometry();
    newParentRect.setHeight(newParentRect.height() * count);
    this->container->setGeometry(newParentRect);

    if (!errors->empty()) {
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
