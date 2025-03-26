#include "dbtree.h"

DbTree::DbTree(QTreeWidget *tree): tree(tree) {
    this->treeNodes = QList<QTreeWidgetItem *>();
}

void DbTree::clear() {
    qDeleteAll(this->treeNodes.begin(), this->treeNodes.end());
    this->treeNodes.clear();
    this->tree->clear();
}

QString getFileSize(const qint64 size) {
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    auto num = static_cast<double>(size);
    while (num >= 1024 && i.hasNext()) {
        unit = i.next();
        num /= 1024.0;
    }

    return QString().setNum(num, 'f', 2) + " " + unit;
}

// ReSharper disable all CppDFAMemoryLeak - By design
// tree nodes are relased in the clean() method
void DbTree::populateTree(const DatabaseInfo& info) {
    this->clear();

    const auto dbInfoNode = new QTreeWidgetItem(this->tree);
    dbInfoNode->setText(0, "Database Info");
    this->treeNodes.prepend(dbInfoNode);

    const auto filenameNode = new QTreeWidgetItem(dbInfoNode);
    filenameNode->setText(0, QString("File name: ").append(info.filename));
    this->treeNodes.prepend(filenameNode);

    const auto creationDateNode = new QTreeWidgetItem(dbInfoNode);
    creationDateNode->setText(0, QString("Created on: ").append(info.creationDate.toLocalTime().toString()));
    this->treeNodes.prepend(creationDateNode);

    const auto sizeNode = new QTreeWidgetItem(dbInfoNode);
    sizeNode->setText(0, QString("File size: ").append(getFileSize(info.size)));
    this->treeNodes.prepend(sizeNode);

    const auto tablesRootNode = new QTreeWidgetItem(this->tree);
    tablesRootNode->setText(0, "Tables");
    this->treeNodes.prepend(tablesRootNode);

    foreach(Table table, info.tables) {
        const auto tableNode = new QTreeWidgetItem(tablesRootNode, QTreeWidgetItem::UserType + 1);
        tableNode->setText(0, table.name);
        this->treeNodes.prepend(tableNode);

        foreach(Column col, table.columns) {
            const auto colName = new QTreeWidgetItem(tableNode);
            colName->setText(0, col.name);
            this->treeNodes.prepend(colName);

            const auto colOrdinal = new QTreeWidgetItem(colName);
            colOrdinal->setText(0, QString("Ordinal Position: ").append(QString::number(col.ordinal)));
            this->treeNodes.prepend(colOrdinal);

            const auto colDataType = new QTreeWidgetItem(colName);
            colDataType->setText(0, QString("Data Type: ").append(col.dataType));
            this->treeNodes.prepend(colDataType);

            const auto colNotNull = new QTreeWidgetItem(colName);
            colNotNull->setText(0, QString("Allow Null: ").append(!col.notNull ? "True" : "False"));
            this->treeNodes.prepend(colNotNull);

            const auto colPrimaryKey = new QTreeWidgetItem(colName);
            colPrimaryKey->setText(0, QString("Is Primary Key: ").append(col.primaryKey ? "True" : "False"));
            this->treeNodes.prepend(colPrimaryKey);

            const auto defaultValue = new QTreeWidgetItem(colName);
            defaultValue->setText(0, QString("Default Value: ").append(col.defaultValue));
            this->treeNodes.prepend(defaultValue);
        }
    }

    this->tree->expandItem(dbInfoNode);
    this->tree->expandItem(tablesRootNode);
}
