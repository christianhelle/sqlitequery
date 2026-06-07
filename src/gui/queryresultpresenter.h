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

private:
    QWidget *widget;
    std::unique_ptr<QScrollArea> scrollArea;
    std::unique_ptr<QWidget> container;
    QList<QTableView *> tableViews{};
};

#endif // QUERYRESULTPRESENTER_H
