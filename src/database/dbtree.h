#ifndef DBTREE_H
#define DBTREE_H

#include <QTreeWidget>

#include "databaseinfo.h"

class DbTree {
public:
    explicit DbTree(QTreeWidget *);

    void clear();

    void populateTree(const DatabaseInfo &info);

private:
    QTreeWidget *tree;
};

#endif // DBTREE_H
