#ifndef PAGEDRESULTMODEL_H
#define PAGEDRESULTMODEL_H

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QString>

// A lazily fetched view over a Query's result set. Rows are read in pages as
// they are scrolled into view, so a result set of any size can be browsed
// without holding it in memory. Sorting re-runs the statement with an ORDER BY
// so that the database does the ordering rather than the rows already fetched.
class PagedResultModel final : public QSqlQueryModel {
    Q_OBJECT

public:
    PagedResultModel(QSqlDatabase database, QString sql, QObject *parent = nullptr);

    // Empty unless the statement failed to execute.
    [[nodiscard]] QString errorText() const { return error; }

    void sort(int column, Qt::SortOrder order) override;

private:
    bool run(const QString &sql);

    QSqlDatabase database;
    QString statement;
    QString error;
};

#endif // PAGEDRESULTMODEL_H
