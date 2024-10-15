#ifndef TIMERTHREAD_H
#define TIMERTHREAD_H

#include <QThread>
#include "timerworker.h"

// Класс для работы с потоками
class TimerThread : public QThread {
    Q_OBJECT

public:
    explicit TimerThread(TimerWorker *worker, QObject *parent = nullptr);

protected:
    void run() override;

private:
    TimerWorker *worker;
};

#endif // TIMERTHREAD_H
