#include <QApplication>
#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
    // Widget tests need a QApplication but no real display.
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "offscreen");

    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    return result;
}
