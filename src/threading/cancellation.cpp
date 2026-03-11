#include "cancellation.h"

CancellationToken::CancellationToken(std::atomic<bool> *isCancelled): isCancelled(isCancelled) {
}

bool CancellationToken::isCancellationRequested() const {
    return isCancelled->load();
}

CancellationToken CancellationTokenSource::get() {
    return CancellationToken(&isCancelled);
}

void CancellationTokenSource::cancel() {
    isCancelled.store(true);
}
