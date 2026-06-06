#ifndef QUERYEXECUTOR_H
#define QUERYEXECUTOR_H

#include "iqueryexecutor.h"

class QueryExecutor : public IQueryExecutor {
public:
    [[nodiscard]] QueryResult execute(IDatabase *database,
                                       const QStringList &queries) const override;
};

#endif // QUERYEXECUTOR_H
