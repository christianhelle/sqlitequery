#ifndef RUN_H
#define RUN_H

#include <QString>


class Run {
public:
    static void run(const QString &sqlFilePath,
                    const QString &dbFilePath);
};

#endif //RUN_H
