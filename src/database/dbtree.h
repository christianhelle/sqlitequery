#ifndef DBTREE_H
#define DBTREE_H

#include <QTreeWidget>

#include "databaseinfo.h"

class DbTree {
public:
    explicit DbTree(QTreeWidget *);

    void clear();

    void populateTree(const DatabaseInfo& info);

private:
    QTreeWidget *tree;
    QList<QTreeWidgetItem *> treeNodes;
};

#endif // DBTREE_H
