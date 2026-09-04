#include "queryresultpresenter.h"

#include <algorithm>

#include <QItemSelectionModel>
#include <QPointer>
#include <QStandardItemModel>

QueryResultPresenter::QueryResultPresenter(QWidget *parent)
    : widget(parent) {
    this->container = new QWidget(this->widget);
    this->container->hide();
    this->scrollArea = new QScrollArea(this->widget);
    this->scrollArea->setWidget(container);
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
    const bool hasResultsToShow = std::any_of(
        results.begin(), results.end(),
        [](const QueryResult &result) { return result.isSelect; });

    // Discard the views (and the models they own) from the previous execution,
    // but only once there is something to replace them with: a statement that
    // renders nothing leaves the previous results on screen.
    if (!hasResultsToShow)
        return;
    this->clear();

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

        const auto tablePtr = new QTableView(container);

        // Parent the model to its view so it is destroyed along with the view.
        auto model = std::make_unique<QStandardItemModel>(
            static_cast<int>(result.rows.size()),
            static_cast<int>(result.columns.size()),
            tablePtr);

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

void QueryResultPresenter::presentToView(QTableView *view, const QueryResult &result) {
    auto model = std::make_unique<QStandardItemModel>(
        static_cast<int>(result.rows.size()),
        static_cast<int>(result.columns.size()),
        view);

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

    // setModel() does not delete the outgoing model, and only clears its
    // selection model in some cases, so drop whatever survives to stop them
    // accumulating on the shared view. QPointer guards against a double delete.
    const QPointer<QAbstractItemModel> previousModel = view->model();
    const QPointer<QItemSelectionModel> previousSelection = view->selectionModel();
    view->setModel(model.release());
    delete previousSelection;
    delete previousModel;
    view->setSortingEnabled(true);
}
