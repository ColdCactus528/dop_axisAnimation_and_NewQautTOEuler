#include "timerthread.h"

TimerThread::TimerThread(TimerWorker *worker, QObject *parent)
    : QThread(parent), worker(worker) {}

void TimerThread::run() {
    worker->moveToThread(this);  // Перемещаем объект TimerWorker в поток
    exec();  // Запускаем event loop потока
}
