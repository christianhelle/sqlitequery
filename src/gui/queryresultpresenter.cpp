#include "queryresultpresenter.h"

#include <QStandardItemModel>

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

    qDeleteAll(this->tableViews.begin(), this->tableViews.end());
    this->tableViews.clear();
}

void QueryResultPresenter::present(const QList<QueryResult> &results) {
    const QRect widgetRect = this->widget->geometry();
    const int width = widgetRect.width();
    const int height = widgetRect.height();

    int yOffset = 0;
    int count = 0;

    for (const auto &result: results) {
        if (!result.isSelect)
            continue;

        if (count > 0)
            yOffset += height;

        auto model = std::make_unique<QStandardItemModel>(
            static_cast<int>(result.rows.size()),
            static_cast<int>(result.columns.size()),
            container.get());

        for (int col = 0; col < result.columns.size(); ++col) {
            model->setHorizontalHeaderItem(col, new QStandardItem(result.columns.at(col)));
        }
        for (int row = 0; row < result.rows.size(); ++row) {
            const auto &values = result.rows.at(row).values;
            for (int col = 0; col < result.columns.size(); ++col) {
                auto *item = new QStandardItem(values.at(col).toString());
                model->setItem(row, col, item);
            }
        }

        const auto tablePtr = new QTableView(container.get());
        tablePtr->setModel(model.release());
        tablePtr->setGeometry(QRect(0, yOffset, width, height));
        tablePtr->show();
        this->tableViews.append(tablePtr);
        count++;
    }

    QRect newParentRect = this->widget->geometry();
    newParentRect.setHeight(newParentRect.height() * count);
    this->container->setGeometry(newParentRect);
}
