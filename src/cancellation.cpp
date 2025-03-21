#include "cancellation.h"

CancellationToken::CancellationToken(bool *isCancelled) {
    this->isCancelled = isCancelled;
}

bool CancellationToken::isCancellationRequested() const {
    return *isCancelled;
}

CancellationToken CancellationTokenSource::get() {
    return CancellationToken(&isCancelled);
}

void CancellationTokenSource::cancel() {
    isCancelled = true;
}
