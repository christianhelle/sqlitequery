#include <gtest/gtest.h>
#include <QTextEdit>
#include <QTreeWidget>

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
