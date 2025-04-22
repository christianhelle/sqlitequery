#ifndef RUN_H
#define RUN_H

#include <QString>


class Runner {
public:
    static void executeSqlFile(const QString &sqlFilePath,
                               const QString &dbFilePath);
};

#endif //RUN_H
