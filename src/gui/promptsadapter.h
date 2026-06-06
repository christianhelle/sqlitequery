#ifndef PROMPTSADAPTER_H
#define PROMPTSADAPTER_H

#include "iprompts.h"
#include "../settings/isettings.h"

class PromptsAdapter : public IPrompts {
public:
    QString getCsvDelimiter(QWidget *parent, const QString &defaultDelimiter) override;
    QString getFolderPath(QWidget *parent) override;
    QString getFilePath(QWidget *parent, QFileDialog::AcceptMode mode) override;
    bool confirmDelete(QWidget *parent, const QString &tableName) override;
    void showError(QWidget *parent, const QString &message) override;

    explicit PromptsAdapter(ISettings *settings = nullptr);

private:
    ISettings *settings;
};

#endif // PROMPTSADAPTER_H
