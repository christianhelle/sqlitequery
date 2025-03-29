#ifndef DBEXPORT_H
#define DBEXPORT_H

#include "database.h"
#include "databaseinfo.h"

class DbExport {
public:
  explicit DbExport(DatabaseInfo info)
    : info(std::move(info)) {
  }

protected:
  DatabaseInfo info;
  static bool isInternalTable(const Table& table);
  QStringList textTypes = {
      "TEXT",
      "CHARACTER",
      "VARCHAR",
      "VARYING CHARACTER",
      "NCHAR",
      "NATIVE CHARACTER",
      "NVARCHAR",
      "CLOB"
  };
};


#endif //DBEXPORT_H
