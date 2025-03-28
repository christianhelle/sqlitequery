#ifndef DBDATAEXPORT_H
#define DBDATAEXPORT_H

#include "dbexport.h"
#include "../threading/cancellation.h"

#include <cstdint>

class ExportDataProgress {
  uint64_t affectedRows = 0;
  bool isComplete = false;

public:
  void reset() {
    affectedRows = 0;
    isComplete = false;
  }

  void increment() {
    affectedRows++;
    isComplete = false;
  }

  [[nodiscard]] uint64_t getAffectedRows() const { return affectedRows; }
  [[nodiscard]] bool isCompleted() const { return isComplete; }
  void setCompleted() { isComplete = true; }
};

class DbDataExport : public DbExport {
public:
  explicit DbDataExport(DatabaseInfo info) : DbExport(info) {}

  [[nodiscard]] QString exportData() const;
  void exportDataToFile(const QString &filename) const;
  void exportDataToFile(const Database *database, const QString &filename,
                        const CancellationToken *cancellationToken,
                        ExportDataProgress *progress) const;

private:
  static QStringList getColumnDefinitions(const Table &table);
  [[nodiscard]] QStringList
  getColumnValueDefinitions(const Table &table, const QSqlQuery &query) const;
};

#endif // DBDATAEXPORT_H
