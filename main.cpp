#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick3D/qquick3d.h>
#include <QQmlContext>
#include "axis.h"
#include "timerworker.h"
#include "timerthread.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());

    qmlRegisterType<Line>("linesExample", 1, 0, "Line");
    qmlRegisterType<Axis>("linesExample", 1, 0, "Axis");
    qmlRegisterType<TimerWorker>("linesExample", 1, 0, "Timerworker");
    qmlRegisterType<TimerThread>("linesExample", 1, 0, "Timerworker");

    QQmlApplicationEngine engine;

    // Axis* axis = new Axis();
    // engine.rootContext()->setContextProperty("newAxis", axis);

    // Создаем объект таймера
    TimerWorker *timerWorker = new TimerWorker(nullptr);

    // Создаем поток для таймера и перемещаем объект TimerWorker в этот поток
    TimerThread *timerThread = new TimerThread(timerWorker);

    timerWorker->moveToThread(timerThread);

    // Запуск потока
    timerThread->start();
    engine.rootContext()->setContextProperty("timerworker", timerWorker);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.loadFromModule("linesExample", "Main");

    return app.exec();
}
