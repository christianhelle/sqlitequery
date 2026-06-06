#ifndef QUERYRESULTPRESENTER_H
#define QUERYRESULTPRESENTER_H

#include "iqueryresultpresenter.h"
#include <QScrollArea>
#include <QTableView>
#include <QSqlQueryModel>
#include <QMessageBox>

class QueryResultPresenter : public IQueryResultPresenter {
public:
    explicit QueryResultPresenter(QWidget *parent);

    void display(const QueryResult &result, QWidget *parent) override;
    void clear() override;

private:
    QWidget *widget;
    std::unique_ptr<QScrollArea> scrollArea;
    std::unique_ptr<QWidget> container;
    QList<QTableView *> tableResults{};

    void layoutResults(const QueryResult &result, QWidget *parent);
};

#endif // QUERYRESULTPRESENTER_H
