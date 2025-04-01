#ifndef PRMOPTS_H
#define PRMOPTS_H

#include <QFileDialog>
#include <QWidget>

class Prompts {
public:
    static QString getCsvDelimiter(QWidget *, const QString &defaultDelimiter);
    static QString getFolderPath(QWidget *parent);
    static QString getFilePath(QWidget *parent, QFileDialog::AcceptMode mode);
};


#endif //PRMOPTS_H
