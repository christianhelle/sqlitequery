
#ifndef EXPORT_H
#define EXPORT_H

#include "../database/dbexportdata.h"

class Export {
public:
    static void exportDataToCsvFile(const QString &file,
                                    const QString &outputFolder,
                                    const QString &delimiter, bool showProgress);
};

#endif // EXPORT_H
