#ifndef CANCELLATION_H
#define CANCELLATION_H


class CancellationToken {
public:
    explicit CancellationToken(bool *isCancelled);

    [[nodiscard]] bool isCancellationRequested() const;

private:
    bool *isCancelled = nullptr;
};

class CancellationTokenSource {
public:
    CancellationToken get();
    void cancel();
private:
    bool isCancelled = false;
};


#endif //CANCELLATION_H
