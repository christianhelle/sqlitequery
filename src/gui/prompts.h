#ifndef PRMOPTS_H
#define PRMOPTS_H

#include <QFileDialog>
#include <QWidget>

#include "mainwindow.h"

class Prompts {
public:
    static QString getCsvDelimiter(QWidget *, const QString &defaultDelimiter);
    static QString getFolderPath(QWidget *parent);
    static QString getFilePath(QWidget *parent, QFileDialog::AcceptMode mode);
    static bool confirmDelete(QWidget *parent, const QString &tableName);
    static void showError(QWidget *parent, const QString &message);
};


#endif //PRMOPTS_H
