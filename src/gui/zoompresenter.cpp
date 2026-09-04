#include "zoompresenter.h"

#include <algorithm>

#include <QAbstractScrollArea>
#include <QApplication>
#include <QEvent>
#include <QFont>
#include <QFontInfo>
#include <QWheelEvent>

namespace {
    // Eighths of a degree in one wheel notch, per Qt's wheel documentation.
    constexpr int notch = 120;
}

ZoomPresenter::ZoomPresenter(QObject *parent) : QObject(parent) {
}

void ZoomPresenter::addTarget(QWidget *target) {
    if (target == nullptr)
        return;

    // Registering a widget twice would capture the size it is *currently*
    // rendered at as its new step 0, and every later zoom would compound from
    // there. Keep the first registration, which holds the real base size.
    for (const auto &existing: targets) {
        if (existing.widget.data() == target)
            return;
    }

    // A widget that inherits its font, or that was sized in pixels, reports no
    // point size of its own. QFontInfo resolves what it is actually rendered
    // at, which is the size the user sees at step 0.
    double basePointSize = target->font().pointSizeF();
    if (basePointSize <= 0)
        basePointSize = QFontInfo(target->font()).pointSizeF();

    targets.append({QPointer<QWidget>(target), basePointSize});
    target->installEventFilter(this);

    // A scrolling widget delivers wheel events to its viewport, not to itself,
    // so that is where the gesture has to be watched for.
    if (const auto *scrollArea = qobject_cast<QAbstractScrollArea *>(target))
        scrollArea->viewport()->installEventFilter(this);

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

bool ZoomPresenter::ownsFocus() const {
    const QWidget *focus = QApplication::focusWidget();
    if (focus == nullptr)
        return false;

    // A scrolling widget can hand focus to a child of its own, so the whole
    // subtree counts as the target holding the focus.
    return std::any_of(targets.begin(), targets.end(), [focus](const Target &target) {
        const QWidget *widget = target.widget.data();
        return widget != nullptr && (widget == focus || widget->isAncestorOf(focus));
    });
}

bool ZoomPresenter::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() != QEvent::Wheel)
        return QObject::eventFilter(watched, event);

    const auto *wheel = static_cast<QWheelEvent *>(event);
    if (!wheel->modifiers().testFlag(Qt::ControlModifier)) {
        bankedWheelDelta = 0;
        return QObject::eventFilter(watched, event);
    }

    bankedWheelDelta += wheel->angleDelta().y();
    while (bankedWheelDelta >= notch) {
        bankedWheelDelta -= notch;
        zoomIn();
    }
    while (bankedWheelDelta <= -notch) {
        bankedWheelDelta += notch;
        zoomOut();
    }

    // Swallow the event so the target zooms instead of also scrolling.
    return true;
}
