#ifndef IDATABASE_H
#define IDATABASE_H

#include <QString>
#include <QList>

class IDatabase {
public:
    virtual ~IDatabase() = default;

    virtual void setSource(const QString &filename) = 0;
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual void shrink() = 0;
    virtual bool isOpen() const = 0;
    [[nodiscard]] virtual QString getFilename() const = 0;
    virtual bool execute(const QString &sql) = 0;
};

#endif // IDATABASE_H
