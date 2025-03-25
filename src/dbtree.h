#ifndef DBTREE_H
#define DBTREE_H

#include <QTreeWidget>

#include "databaseinfo.h"

class DbTree {
public:
    explicit DbTree(QTreeWidget *);

    void clear() const;

    void populateTree(const DatabaseInfo& info) const;

private:
    QTreeWidget *tree;
    QList<QTreeWidgetItem *> *treeNodes;
};

#endif // DBTREE_H
