#include "dbtree.h"

DbTree::DbTree(QTreeWidget *tree) {
    this->tree = tree;
    this->treeNodes = new QList<QTreeWidgetItem *>();
}

DbTree::~DbTree() {
    this->clear();
}

void DbTree::clear() {
    this->tree->clear();
    //    qDeleteAll(this->treeNodes->begin(), this->treeNodes->end());
    //    this->treeNodes->clear();
}

QString getFileSize(qint64 size) {
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while (num >= 1024 && i.hasNext()) {
        unit = i.next();
        num /= 1024.0;
    }

    return QString().setNum(num, 'f', 2) + " " + unit;
}

void DbTree::populateTree(DatabaseInfo info) {
    this->clear();

    QTreeWidgetItem *dbInfoNode = new QTreeWidgetItem(this->tree);
    dbInfoNode->setText(0, "Database Info");
    this->treeNodes->prepend(dbInfoNode);

    QTreeWidgetItem *filenameNode = new QTreeWidgetItem(dbInfoNode);
    filenameNode->setText(0, QString("File name: ").append(info.filename));
    this->treeNodes->prepend(filenameNode);

    QTreeWidgetItem *creationDateNode = new QTreeWidgetItem(dbInfoNode);
    creationDateNode->setText(0, QString("Created on: ").append(info.creationDate.toLocalTime().toString()));
    this->treeNodes->prepend(creationDateNode);

    QTreeWidgetItem *sizeNode = new QTreeWidgetItem(dbInfoNode);
    sizeNode->setText(0, QString("File size: ").append(getFileSize(info.size)));
    this->treeNodes->prepend(sizeNode);

    QTreeWidgetItem *tablesRootNode = new QTreeWidgetItem(this->tree);
    tablesRootNode->setText(0, "Tables");
    this->treeNodes->prepend(tablesRootNode);

    foreach(Table table, info.tables) {
        QTreeWidgetItem *tableNode = new QTreeWidgetItem(tablesRootNode, QTreeWidgetItem::UserType + 1);
        tableNode->setText(0, table.name);
        this->treeNodes->prepend(tableNode);

        foreach(Column col, table.columns) {
            QTreeWidgetItem *colName = new QTreeWidgetItem(tableNode);
            colName->setText(0, col.name);
            this->treeNodes->prepend(colName);

            QTreeWidgetItem *colOrdinal = new QTreeWidgetItem(colName);
            colOrdinal->setText(0, QString("Ordinal Position: ").append(QString::number(col.ordinal)));
            this->treeNodes->prepend(colOrdinal);

            QTreeWidgetItem *colDataType = new QTreeWidgetItem(colName);
            colDataType->setText(0, QString("Data Type: ").append(col.dataType));
            this->treeNodes->prepend(colDataType);

            QTreeWidgetItem *colNotNull = new QTreeWidgetItem(colName);
            colNotNull->setText(0, QString("Allow Null: ").append(!col.notNull ? "True" : "False"));
            this->treeNodes->prepend(colNotNull);
        }
    }

    this->tree->expandItem(dbInfoNode);
    this->tree->expandItem(tablesRootNode);
}
