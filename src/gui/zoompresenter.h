#ifndef ZOOMPRESENTER_H
#define ZOOMPRESENTER_H

#include <QList>
#include <QObject>
#include <QPointer>
#include <QWidget>

#include "zoomlevel.h"

// Applies a Zoom to the widgets it is given. Each widget keeps the font size
// it was built with as its own step 0, so widgets that start at different
// sizes stay proportional to each other and repeated zooming never drifts.
class ZoomPresenter final : public QObject {
    Q_OBJECT

public:
    explicit ZoomPresenter(QObject *parent = nullptr);

    // Registers a widget whose text scales with the Zoom, and immediately
    // renders it at the current step.
    void addTarget(QWidget *target);

    void zoomIn();

    void zoomOut();

    void resetZoom();

    [[nodiscard]] int step() const { return level.step(); }

    void setStep(int step);

    // Turns Ctrl+wheel over a target into zoom steps instead of scrolling.
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void zoomChanged(int step);

private:
    void applyToTargets() const;

    struct Target {
        QPointer<QWidget> widget;
        double basePointSize;
    };

    ZoomLevel level;
    QList<Target> targets;
    // A wheel notch is 120 eighths of a degree. Trackpads report far smaller
    // amounts, so they are banked until they add up to a notch.
    int bankedWheelDelta = 0;
};

#endif // ZOOMPRESENTER_H
