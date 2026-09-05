#include <gtest/gtest.h>
#include <QApplication>
#include <QTextEdit>
#include <QTreeWidget>
#include <QWheelEvent>

#include "gui/zoompresenter.h"

class ZoomPresenterTest : public ::testing::Test {
protected:
    void SetUp() override {
        editor = std::make_unique<QTextEdit>();
        QFont font = editor->font();
        font.setPointSizeF(10.0);
        editor->setFont(font);

        tree = std::make_unique<QTreeWidget>();
        QFont treeFont = tree->font();
        treeFont.setPointSizeF(20.0);
        tree->setFont(treeFont);

        presenter = std::make_unique<ZoomPresenter>();
        presenter->addTarget(editor.get());
        presenter->addTarget(tree.get());
    }

    static double pointSize(const QWidget *widget) {
        return widget->font().pointSizeF();
    }

    static void wheel(QWidget *target, const int angleDelta, const Qt::KeyboardModifiers modifiers) {
        QWheelEvent event(QPointF(1, 1), target->mapToGlobal(QPointF(1, 1)),
                          QPoint(0, 0), QPoint(0, angleDelta),
                          Qt::NoButton, modifiers, Qt::NoScrollPhase, false);
        QApplication::sendEvent(target, &event);
    }

    std::unique_ptr<QTextEdit> editor;
    std::unique_ptr<QTreeWidget> tree;
    std::unique_ptr<ZoomPresenter> presenter;
};

TEST_F(ZoomPresenterTest, LeavesTargetsAloneUntilZoomed) {
    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 10.0);
    EXPECT_DOUBLE_EQ(pointSize(tree.get()), 20.0);
}

TEST_F(ZoomPresenterTest, ZoomInGrowsEveryTargetFromItsOwnSize) {
    presenter->zoomIn();

    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 11.0);
    EXPECT_DOUBLE_EQ(pointSize(tree.get()), 22.0);
}

TEST_F(ZoomPresenterTest, ZoomOutShrinksEveryTarget) {
    presenter->zoomOut();

    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 9.0);
    EXPECT_DOUBLE_EQ(pointSize(tree.get()), 18.0);
}

TEST_F(ZoomPresenterTest, ZoomingBackAndForthDoesNotDrift) {
    for (int i = 0; i < 5; ++i)
        presenter->zoomIn();
    for (int i = 0; i < 5; ++i)
        presenter->zoomOut();

    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 10.0);
    EXPECT_DOUBLE_EQ(pointSize(tree.get()), 20.0);
}

TEST_F(ZoomPresenterTest, ResetReturnsTargetsToTheirOriginalSize) {
    presenter->zoomIn();
    presenter->zoomIn();
    presenter->resetZoom();

    EXPECT_EQ(presenter->step(), 0);
    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 10.0);
    EXPECT_DOUBLE_EQ(pointSize(tree.get()), 20.0);
}

TEST_F(ZoomPresenterTest, RendersALateTargetAtTheCurrentStep) {
    presenter->zoomIn();

    QTextEdit late;
    QFont font = late.font();
    font.setPointSizeF(10.0);
    late.setFont(font);
    presenter->addTarget(&late);

    EXPECT_DOUBLE_EQ(pointSize(&late), 11.0);
}

TEST_F(ZoomPresenterTest, ReportsEachChangeOfStep) {
    QList<int> reported;
    QObject::connect(presenter.get(), &ZoomPresenter::zoomChanged,
                     [&reported](const int step) { reported.append(step); });

    presenter->zoomIn();
    presenter->zoomOut();

    ASSERT_EQ(reported.size(), 2);
    EXPECT_EQ(reported.at(0), 1);
    EXPECT_EQ(reported.at(1), 0);
}

TEST_F(ZoomPresenterTest, StaysSilentAtTheEndOfTheRange) {
    presenter->setStep(ZoomLevel::maximumStep);

    int reports = 0;
    QObject::connect(presenter.get(), &ZoomPresenter::zoomChanged,
                     [&reports](int) { ++reports; });
    presenter->zoomIn();

    EXPECT_EQ(reports, 0);
}

TEST_F(ZoomPresenterTest, SurvivesATargetThatWasDestroyed) {
    {
        QTextEdit shortLived;
        presenter->addTarget(&shortLived);
    }

    presenter->zoomIn();

    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 11.0);
}

TEST_F(ZoomPresenterTest, IgnoresAMissingTarget) {
    presenter->addTarget(nullptr);
    presenter->zoomIn();

    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 11.0);
}

TEST_F(ZoomPresenterTest, ControlWheelUpOverTheEditorZoomsIn) {
    wheel(editor->viewport(), 120, Qt::ControlModifier);

    EXPECT_EQ(presenter->step(), 1);
    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 11.0);
}

TEST_F(ZoomPresenterTest, ControlWheelDownOverTheTreeZoomsOut) {
    wheel(tree->viewport(), -120, Qt::ControlModifier);

    EXPECT_EQ(presenter->step(), -1);
    EXPECT_DOUBLE_EQ(pointSize(tree.get()), 18.0);
}

TEST_F(ZoomPresenterTest, WheelWithoutControlDoesNotZoom) {
    wheel(editor->viewport(), 120, Qt::NoModifier);

    EXPECT_EQ(presenter->step(), 0);
    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 10.0);
}

TEST_F(ZoomPresenterTest, BanksTrackpadScrollingIntoWholeSteps) {
    for (int i = 0; i < 3; ++i)
        wheel(editor->viewport(), 30, Qt::ControlModifier);
    EXPECT_EQ(presenter->step(), 0);

    wheel(editor->viewport(), 30, Qt::ControlModifier);
    EXPECT_EQ(presenter->step(), 1);
}

TEST_F(ZoomPresenterTest, ForgetsBankedScrollingWhenControlIsReleased) {
    wheel(editor->viewport(), 90, Qt::ControlModifier);
    wheel(editor->viewport(), 90, Qt::NoModifier);
    wheel(editor->viewport(), 90, Qt::ControlModifier);

    EXPECT_EQ(presenter->step(), 0);
}

TEST_F(ZoomPresenterTest, RegisteringTheSameTargetTwiceDoesNotCompound) {
    presenter->zoomIn();

    // The editor is already registered and already scaled. Registering it again
    // must not treat its current size as the size to scale from.
    presenter->addTarget(editor.get());
    presenter->zoomIn();

    EXPECT_DOUBLE_EQ(pointSize(editor.get()), 12.0);
}

TEST_F(ZoomPresenterTest, DoesNotOwnFocusWhenNothingIsFocused) {
    EXPECT_FALSE(presenter->ownsFocus());
}

TEST_F(ZoomPresenterTest, OwnsFocusWhenATargetIsFocused) {
    editor->show();
    editor->setFocus();
    QApplication::processEvents();

    ASSERT_EQ(QApplication::focusWidget(), editor.get());
    EXPECT_TRUE(presenter->ownsFocus());
}

TEST_F(ZoomPresenterTest, DoesNotOwnFocusWhenAnUnregisteredWidgetIsFocused) {
    QTextEdit stranger;
    stranger.show();
    stranger.setFocus();
    QApplication::processEvents();

    ASSERT_EQ(QApplication::focusWidget(), &stranger);
    EXPECT_FALSE(presenter->ownsFocus());
}
