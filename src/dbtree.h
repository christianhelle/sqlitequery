#ifndef DBTREE_H
#define DBTREE_H

#include <QWidget>
#include <QTreeWidget>

#include "databaseinfo.h"

class DbTree {
public:
    DbTree(QTreeWidget *);

    ~DbTree();

    void clear() const;

    void populateTree(const DatabaseInfo& info);

private:
    QTreeWidget *tree;
    QList<QTreeWidgetItem *> *treeNodes;
};

#endif // DBTREE_H
