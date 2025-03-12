#include <QApplication>
#include "mainwindow.h"

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    int argc = __argc;
    char** argv = __argv;
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
#else
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
#endif
