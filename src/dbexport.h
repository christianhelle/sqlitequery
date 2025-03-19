#ifndef DBEXPORT_H
#define DBEXPORT_H

#include "database.h"


class DbExport {
public:
    explicit DbExport(DatabaseInfo info)
        : info(std::move(info)) {
    }


private:
    DatabaseInfo info;
};


#endif //DBEXPORT_H
