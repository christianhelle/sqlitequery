#ifndef PROGRESS_H
#define PROGRESS_H

#include <QStringList>
#include <QTextStream>
#include <stdio.h>

class ExportDataProgress {
    uint64_t affectedRows = 0;
    bool isComplete = false;
    bool printProgress = false;
    bool hasErrors = false;
    QStringList errors;

public:
    void reset() {
        affectedRows = 0;
        isComplete = false;
        hasErrors = false;
        errors.clear();
    }

    void increment() {
        affectedRows++;
        isComplete = false;
        if (printProgress) {
            QTextStream out(stdout);
            out << "\rProgress: " << affectedRows << " rows processed";
            fflush(stdout);
        }
    }

    void addError(const QString &table, const QString &message) {
        hasErrors = true;
        errors.append(QString("[%1] %2").arg(table, message));
    }

    [[nodiscard]] uint64_t getAffectedRows() const { return affectedRows; }
    [[nodiscard]] bool isCompleted() const { return isComplete; }
    [[nodiscard]] bool hasAnyErrors() const { return hasErrors; }
    [[nodiscard]] const QStringList &getErrors() const { return errors; }
    void setCompleted() { isComplete = true; }
    void setShowProgress(const bool value) { printProgress = value; }
    [[nodiscard]] bool isShowProgress() const { return printProgress; }
};

#endif //PROGRESS_H
