#ifndef IQUERYRESULTPRESENTER_H
#define IQUERYRESULTPRESENTER_H

#include <QWidget>
#include "queryresult.h"

class IQueryResultPresenter {
public:
    virtual ~IQueryResultPresenter() = default;

    virtual void display(const QueryResult &result, QWidget *parent) = 0;
    virtual void clear() = 0;
};

#endif // IQUERYRESULTPRESENTER_H
