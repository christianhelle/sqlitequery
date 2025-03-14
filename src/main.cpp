#include <QApplication>
#include "mainwindow.h"

#define VERSION "1.0.0"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setApplicationVersion(VERSION);

    MainWindow w;

    QStringList args = a.arguments();
    if (args.length() > 1) {
        w.openDatabase(args.takeAt(1));
    }

    w.show();
    return a.exec();
}
