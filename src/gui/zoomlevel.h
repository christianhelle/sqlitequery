#ifndef ZOOMLEVEL_H
#define ZOOMLEVEL_H

// A Zoom expressed as a discrete step away from the font a widget was built
// with. Step 0 is that font untouched; every step is ten percent of it. The
// range is clamped so text can neither shrink to nothing nor grow until a
// single row fills the window.
class ZoomLevel {
public:
    static constexpr int minimumStep = -5;
    static constexpr int maximumStep = 20;

    ZoomLevel() = default;

    explicit ZoomLevel(int step);

    [[nodiscard]] int step() const { return currentStep; }

    // The multiplier to apply to a font size at the current step.
    [[nodiscard]] double scale() const;

    // Each of these reports whether the step actually moved, so a caller can
    // skip re-applying fonts when the level is already at the end of its range.
    bool zoomIn();

    bool zoomOut();

    bool reset();

    bool setStep(int step);

private:
    int currentStep = 0;
};

#endif // ZOOMLEVEL_H
