#ifndef IQUERYEXECUTOR_H
#define IQUERYEXECUTOR_H

#include "queryresult.h"
#include "idatabase.h"

class IQueryExecutor {
public:
    virtual ~IQueryExecutor() = default;

    [[nodiscard]] virtual QueryResult execute(IDatabase *database,
                                               const QStringList &queries) const = 0;
};

#endif // IQUERYEXECUTOR_H
