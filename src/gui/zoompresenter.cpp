#include "zoompresenter.h"

#include <QFont>
#include <QFontInfo>

ZoomPresenter::ZoomPresenter(QObject *parent) : QObject(parent) {
}

void ZoomPresenter::addTarget(QWidget *target) {
    if (target == nullptr)
        return;

    // A widget that inherits its font, or that was sized in pixels, reports no
    // point size of its own. QFontInfo resolves what it is actually rendered
    // at, which is the size the user sees at step 0.
    double basePointSize = target->font().pointSizeF();
    if (basePointSize <= 0)
        basePointSize = QFontInfo(target->font()).pointSizeF();

    targets.append({QPointer<QWidget>(target), basePointSize});
    applyToTargets();
}

void ZoomPresenter::zoomIn() {
    if (level.zoomIn()) {
        applyToTargets();
        emit zoomChanged(level.step());
    }
}

void ZoomPresenter::zoomOut() {
    if (level.zoomOut()) {
        applyToTargets();
        emit zoomChanged(level.step());
    }
}

void ZoomPresenter::resetZoom() {
    if (level.reset()) {
        applyToTargets();
        emit zoomChanged(level.step());
    }
}

void ZoomPresenter::setStep(const int step) {
    if (level.setStep(step)) {
        applyToTargets();
        emit zoomChanged(level.step());
    }
}

void ZoomPresenter::applyToTargets() const {
    const double scale = level.scale();
    for (const auto &[widget, basePointSize]: targets) {
        // A target can outlive its registration if the window tears down
        // around it, so skip whatever has already gone.
        if (widget.isNull())
            continue;

        QFont font = widget->font();
        font.setPointSizeF(basePointSize * scale);
        widget->setFont(font);
    }
}
