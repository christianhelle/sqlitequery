#ifndef CANCELLATION_H
#define CANCELLATION_H

#include <atomic>

class CancellationToken {
public:
    explicit CancellationToken(std::atomic<bool> *isCancelled);

    [[nodiscard]] bool isCancellationRequested() const;

private:
    std::atomic<bool> *isCancelled = nullptr;
};

class CancellationTokenSource {
public:
    CancellationToken get();

    void cancel();

private:
    std::atomic<bool> isCancelled{false};
};


#endif //CANCELLATION_H
