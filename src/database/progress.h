#ifndef PROGRESS_H
#define PROGRESS_H

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

#endif //PROGRESS_H
