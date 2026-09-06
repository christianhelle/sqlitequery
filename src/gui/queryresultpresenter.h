#ifndef QUERYRESULTPRESENTER_H
#define QUERYRESULTPRESENTER_H

#include <QList>
#include <QScrollArea>
#include <QTableView>
#include <QWidget>
#include <memory>

class QAbstractItemModel;

class QueryResultPresenter {
public:
    explicit QueryResultPresenter(QWidget *parent);

    void clear();

    // Takes ownership of the models and binds each to its own table view.
    void present(const QList<QAbstractItemModel *> &models);

    // Takes ownership of the model and binds it to the given view.
    void presentToView(QTableView *view, QAbstractItemModel *model);

    // The views holding the current results. Rebuilt by every present(), so a
    // caller that wants to do something to them re-reads this after each run.
    [[nodiscard]] const QList<QTableView *> &views() const { return tableViews; }

private:
    QWidget *widget;
    // Owned by the parent widget, as with every other widget in the tree.
    QScrollArea *scrollArea;
    QWidget *container;
    QList<QTableView *> tableViews{};
};

#endif // QUERYRESULTPRESENTER_H
