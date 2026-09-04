#ifndef QUERYRESULTPRESENTER_H
#define QUERYRESULTPRESENTER_H

#include <QList>
#include <QScrollArea>
#include <QTableView>
#include <QWidget>
#include <memory>

#include "../database/queryresult.h"

class QueryResultPresenter {
public:
    explicit QueryResultPresenter(QWidget *parent);

    void clear();

    void present(const QList<QueryResult> &results);

    void presentToView(QTableView *view, const QueryResult &result);

private:
    QWidget *widget;
    // Owned by the parent widget, as with every other widget in the tree.
    QScrollArea *scrollArea;
    QWidget *container;
    QList<QTableView *> tableViews{};
};

#endif // QUERYRESULTPRESENTER_H
