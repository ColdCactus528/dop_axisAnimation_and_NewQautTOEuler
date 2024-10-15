#include "timerworker.h"
#include <QDebug>
#include <QThread>

TimerWorker::TimerWorker(QObject *parent) : QObject(parent), timer(nullptr), m_timerValue(0) {
    // Подключаем сигнал к слоту для создания таймера в правильном потоке
    connect(this, &TimerWorker::startTimerSignal, this, &TimerWorker::initializeTimer);
}

void TimerWorker::startTimer(int interval, int end) {
    emit startTimerSignal(interval, end);  // Эмитируем сигнал для инициализации таймера
}

void TimerWorker::initializeTimer(int interval, int end) {
    // Проверяем, что таймер создаётся в правильном потоке
    qDebug() << "Создание таймера в потоке: " << QThread::currentThread();

    if (!timer) {
        timer = new QTimer(this);  // Создаем таймер с родителем TimerWorker
        connect(timer, &QTimer::timeout, this, &TimerWorker::onTimeout);
        qDebug() << "Таймер готов";
    }

    if (timer->isActive())
        timer->stop();

    m_timerValue = 0;  // Сбрасываем значение
    emit timerValueChanged();  // Уведомляем об изменении значения

    timer->start(interval);  // Запускаем таймер

    // Таймер для авто-выключения через 30 секунд
    QTimer::singleShot(end, this, &TimerWorker::stopTimer);
}

void TimerWorker::stopTimer() {
    if (timer && timer->isActive()) {
        timer->stop();
        emit timerStopped();
        // Отправляем сигнал о том, что таймер остановлен
        qDebug() << "Таймер остановлен, " << "Значение таймера = " << m_timerValue ;
    }
}

int TimerWorker::timerValue() const {
    return m_timerValue;
}

bool TimerWorker::isTimerActive() const {
    return timer ? timer->isActive() : false;
}

void TimerWorker::onTimeout() {
    m_timerValue++;  // Увеличиваем значение на 1 каждую секунду
    emit timerValueChanged();  // Уведомляем об изменении значения
    emit timeoutSignal();  // Отправляем сигнал в QML
}
