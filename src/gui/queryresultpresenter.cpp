#include "queryresultpresenter.h"

#include <algorithm>

#include <QItemSelectionModel>
#include <QPointer>
#include <QAbstractItemModel>
#include <QHeaderView>

namespace {
    // Qt starts a header off pointing at the first column in descending order,
    // and enabling sorting on a view sorts whatever model is bound to it there
    // and then. A paged result answers a sort by re-running its statement
    // wrapped in an ORDER BY, so every result used to be executed twice and
    // shown backwards before anyone had asked for an order -- and the wrap
    // undoes the paging, because the database has to order the whole result set
    // before it can hand back the first page.
    //
    // Clearing the indicator first leaves the rows in the order the statement
    // produced them and makes a sort request for column -1, which the paged
    // model ignores. The first click on any column then sorts ascending, which
    // is Qt's default order for a section the indicator is not already on.
    void enableSortingAscending(QTableView *view) {
        view->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
        view->setSortingEnabled(true);
    }
}

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

void QueryResultPresenter::present(const QList<QAbstractItemModel *> &models) {
    // Discard the views (and the models they own) from the previous execution,
    // but only once there is something to replace them with: a statement that
    // renders nothing leaves the previous results on screen.
    if (models.isEmpty())
        return;
    this->clear();

    const QRect widgetRect = this->widget->geometry();
    const int width = widgetRect.width();
    const int height = widgetRect.height();

    int yOffset = 0;
    int count = 0;

    for (auto *model: models) {
        if (count > 0)
            yOffset += height;

        const auto tablePtr = new QTableView(container);

        // Parent the model to its view so it is destroyed along with the view.
        model->setParent(tablePtr);
        // Sorting is enabled before the model is bound, so binding it does not
        // sort it.
        enableSortingAscending(tablePtr);
        tablePtr->setModel(model);
        tablePtr->setGeometry(QRect(0, yOffset, width, height));
        tablePtr->show();
        this->tableViews.append(tablePtr);
        count++;
    }

    QRect newParentRect = this->widget->geometry();
    newParentRect.setHeight(newParentRect.height() * count);
    this->container->setGeometry(newParentRect);
}

void QueryResultPresenter::presentToView(QTableView *view, QAbstractItemModel *model) {
    if (model == nullptr)
        return;

    // setModel() does not delete the outgoing model, and only clears its
    // selection model in some cases, so drop whatever survives to stop them
    // accumulating on the shared view. QPointer guards against a double delete.
    const QPointer<QAbstractItemModel> previousModel = view->model();
    const QPointer<QItemSelectionModel> previousSelection = view->selectionModel();

    model->setParent(view);
    // As in present(): sorting is set up while the outgoing model is still
    // bound, because doing it afterwards would sort the table on the way in.
    enableSortingAscending(view);
    view->setModel(model);
    delete previousSelection.data();
    delete previousModel.data();
}
