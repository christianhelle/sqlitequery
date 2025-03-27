#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <QGuiApplication>
#include <QObject>

class MainThread {
public:
    template<typename F>
    static void run(F &&fun) {
        QObject tmp;
        QObject::connect(&tmp,
                         &QObject::destroyed,
                         qApp,
                         std::forward<F>(fun),
                         Qt::QueuedConnection);
    }
};


#endif //MAINTHREAD_H
