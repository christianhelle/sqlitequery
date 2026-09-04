#include <gtest/gtest.h>

#include "gui/zoomlevel.h"

TEST(ZoomLevelTest, StartsUnscaled) {
    const ZoomLevel level;
    EXPECT_EQ(level.step(), 0);
    EXPECT_DOUBLE_EQ(level.scale(), 1.0);
}

TEST(ZoomLevelTest, ZoomInGrowsTextByTenPercentPerStep) {
    ZoomLevel level;
    EXPECT_TRUE(level.zoomIn());
    EXPECT_TRUE(level.zoomIn());
    EXPECT_EQ(level.step(), 2);
    EXPECT_DOUBLE_EQ(level.scale(), 1.2);
}

TEST(ZoomLevelTest, ZoomOutShrinksTextByTenPercentPerStep) {
    ZoomLevel level;
    EXPECT_TRUE(level.zoomOut());
    EXPECT_EQ(level.step(), -1);
    EXPECT_DOUBLE_EQ(level.scale(), 0.9);
}

TEST(ZoomLevelTest, ResetReturnsToTheOriginalSize) {
    ZoomLevel level(7);
    EXPECT_TRUE(level.reset());
    EXPECT_EQ(level.step(), 0);
    EXPECT_DOUBLE_EQ(level.scale(), 1.0);
}

TEST(ZoomLevelTest, ResetAtTheOriginalSizeChangesNothing) {
    ZoomLevel level;
    EXPECT_FALSE(level.reset());
}

TEST(ZoomLevelTest, StopsAtTheSmallestStep) {
    ZoomLevel level(ZoomLevel::minimumStep);
    EXPECT_FALSE(level.zoomOut());
    EXPECT_EQ(level.step(), ZoomLevel::minimumStep);
    EXPECT_GT(level.scale(), 0.0);
}

TEST(ZoomLevelTest, StopsAtTheLargestStep) {
    ZoomLevel level(ZoomLevel::maximumStep);
    EXPECT_FALSE(level.zoomIn());
    EXPECT_EQ(level.step(), ZoomLevel::maximumStep);
}

TEST(ZoomLevelTest, ClampsAStepFromOutsideTheRange) {
    const ZoomLevel tooSmall(ZoomLevel::minimumStep - 100);
    EXPECT_EQ(tooSmall.step(), ZoomLevel::minimumStep);

    const ZoomLevel tooLarge(ZoomLevel::maximumStep + 100);
    EXPECT_EQ(tooLarge.step(), ZoomLevel::maximumStep);
}

TEST(ZoomLevelTest, SettingTheSameStepChangesNothing) {
    ZoomLevel level(3);
    EXPECT_FALSE(level.setStep(3));
    EXPECT_TRUE(level.setStep(4));
}
