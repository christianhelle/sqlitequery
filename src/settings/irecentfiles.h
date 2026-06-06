#ifndef IRECENTFILES_H
#define IRECENTFILES_H

#include <QString>
#include <QStringList>

class IRecentFiles {
public:
    virtual ~IRecentFiles() = default;

    virtual void add(const QString &filepath) = 0;
    [[nodiscard]] virtual QStringList list() = 0;
    virtual void clear() = 0;
};

#endif // IRECENTFILES_H
