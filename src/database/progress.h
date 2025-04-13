#ifndef PROGRESS_H
#define PROGRESS_H

class ExportDataProgress {
    uint64_t affectedRows = 0;
    bool isComplete = false;
    bool printProgress = false;

public:
    void reset() {
        affectedRows = 0;
        isComplete = false;
    }

    void increment() {
        affectedRows++;
        isComplete = false;
        if (printProgress) {
            printf("Progress: %llu rows processed.\n", affectedRows);
        }
    }

    [[nodiscard]] uint64_t getAffectedRows() const { return affectedRows; }
    [[nodiscard]] bool isCompleted() const { return isComplete; }
    void setCompleted() { isComplete = true; }
    void setShowProgress(bool value) { printProgress = value; }
    [[nodiscard]] bool isShowProgress() const { return printProgress; }
};

#endif //PROGRESS_H
