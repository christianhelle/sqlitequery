#include "queryresultpresenter.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QRect>

QueryResultPresenter::QueryResultPresenter(QWidget *parent)
    : widget(parent) {
    this->container = std::make_unique<QWidget>(this->widget);
    this->container->hide();
    this->scrollArea = std::make_unique<QScrollArea>(this->widget);
    this->scrollArea->setWidget(container.get());
    this->scrollArea->hide();
}

void QueryResultPresenter::clear() {
    this->container->setGeometry(this->widget->geometry());
    this->container->show();

    this->scrollArea->setGeometry(this->widget->geometry());
    this->scrollArea->show();

    qDeleteAll(this->tableResults.begin(), this->tableResults.end());
    this->tableResults.clear();
}

void QueryResultPresenter::display(const QueryResult &result, QWidget *parent) {
    this->clear();

    if (!result.success) {
        QString msg;
        for (const auto &error : result.errors) {
            msg += error;
            msg += "\r\n";
        }
        QMessageBox::information(parent, "Error", msg, QMessageBox::Ok);
        return;
    }

    if (result.rows.isEmpty()) {
        return;
    }

    QRect widgetRect = this->widget->geometry();
    int yOffset = 0;
    const int width = widgetRect.width();
    const int height = widgetRect.height();

    auto rect = QRect(0, yOffset, width, height);
    const auto ptr = this->container.get();
    auto table = std::make_unique<QTableView>(ptr);
    table->setGeometry(rect);
    table->show();
    const auto tablePtr = table.get();

    this->tableResults.append(table.release());

    auto model = std::make_unique<QSqlQueryModel>(tablePtr);
    // Store the result rows in the model
    QStandardItemModel *standardModel = new QStandardItemModel(0, 0, tablePtr);
    if (!result.rows.isEmpty()) {
        standardModel->setHorizontalHeaderLabels(
            result.rows.first().values
                .indexed()
                .keys()
                .transform([](const auto &k) { return k.toString(); })
                .toList()
        );
        for (const auto &row : result.rows) {
            QList<QStandardItem *> items;
            for (const auto &value : row.values) {
                items.append(new QStandardItem(value.toString()));
            }
            standardModel->appendRow(items);
        }
    }
    tablePtr->setModel(standardModel);
    tablePtr->repaint();
}

void QueryResultPresenter::layoutResults(const QueryResult &result, QWidget *parent) {
    Q_UNUSED(result)
    Q_UNUSED(parent)
}
