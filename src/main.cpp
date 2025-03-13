#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    QStringList args = a.arguments();
    if (args.length() > 1) {
        w.openDatabase(args.takeAt(1));
    }

    w.show();
    return a.exec();
}
