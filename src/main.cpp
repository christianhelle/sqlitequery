#include <QApplication>
#include "gui/mainwindow.h"

#define VERSION "1.0.0"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationVersion(VERSION);

    MainWindow window;
    if (QStringList args = QApplication::arguments(); args.length() > 1) {
        window.openDatabase(args.takeAt(1));
    } else {
        window.restoreLastSession();
    }

    window.show();
    return QApplication::exec();
}
