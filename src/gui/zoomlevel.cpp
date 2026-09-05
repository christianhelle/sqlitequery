#include "zoomlevel.h"

#include <algorithm>

namespace {
    int clamped(const int step) {
        return std::clamp(step, ZoomLevel::minimumStep, ZoomLevel::maximumStep);
    }
}

ZoomLevel::ZoomLevel(const int step) : currentStep(clamped(step)) {
}

double ZoomLevel::scale() const {
    return 1.0 + 0.1 * currentStep;
}

bool ZoomLevel::zoomIn() {
    return setStep(currentStep + 1);
}

bool ZoomLevel::zoomOut() {
    return setStep(currentStep - 1);
}

bool ZoomLevel::reset() {
    return setStep(0);
}

bool ZoomLevel::setStep(const int step) {
    const int next = clamped(step);
    if (next == currentStep)
        return false;
    currentStep = next;
    return true;
}
