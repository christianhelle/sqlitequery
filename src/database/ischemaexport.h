#ifndef ISCHEMASCRIPT_H
#define ISCHEMASCRIPT_H

#include <QString>

class ISchemaScript {
public:
    virtual ~ISchemaScript() = default;
    [[nodiscard]] virtual QString exportSchema() const = 0;
};

#endif // ISCHEMASCRIPT_H
