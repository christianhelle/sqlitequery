#ifndef IPROMPTS_H
#define IPROMPTS_H

#include <QString>
#include <QFileDialog>
#include <QWidget>

class IPrompts {
public:
    virtual ~IPrompts() = default;

    [[nodiscard]] virtual QString getCsvDelimiter(QWidget *parent, const QString &defaultDelimiter) = 0;
    [[nodiscard]] virtual QString getFolderPath(QWidget *parent) = 0;
    [[nodiscard]] virtual QString getFilePath(QWidget *parent, QFileDialog::AcceptMode mode) = 0;
    [[nodiscard]] virtual bool confirmDelete(QWidget *parent, const QString &tableName) = 0;
    virtual void showError(QWidget *parent, const QString &message) = 0;
};

#endif // IPROMPTS_H
