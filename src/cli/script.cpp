#include "script.h"

#include <QFile>
#include <QSqlError>

#include "../database/database.h"
#include "../database/dbanalyzer.h"

void Script::executeSqlFile(const QString &sqlFilePath,
                            const QString &dbFilePath) {
    const auto sqlFile = std::make_unique<QFile>(sqlFilePath);
    if (!sqlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Unable to open SQL file");
        return;
    }

    QTextStream in(sqlFile.get());
    const QString sqlScript = in.readAll();
    const QStringList queryList(sqlScript.split(";", Qt::SkipEmptyParts));
    QStringList errors;

    const auto database = std::make_unique<Database>();
    database->setSource(dbFilePath);
    if (!database->open()) {
        qWarning("Unable to open database file");
        return;
    }

    const QSqlDatabase db = database->getDatabase();
    for (QString sql: queryList) {
        sql = sql.trimmed().replace('\n', "", Qt::CaseInsensitive);
        if (sql.isEmpty())
            continue;

        if (QSqlQuery query(db); !query.exec(sql)) {
            QSqlError error;
            errors.append((error = db.lastError()).isValid()
                              ? error.text()
                              : "Query execution failed");
        }
    }
}
