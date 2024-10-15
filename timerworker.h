#ifndef TIMERWORKER_H
#define TIMERWORKER_H

#include <QObject>
#include <QTimer>

class TimerWorker : public QObject {
    Q_OBJECT
    Q_PROPERTY(int timerValue READ timerValue NOTIFY timerValueChanged)  // Добавляем свойство

public:
    explicit TimerWorker(QObject *parent = nullptr);

    Q_INVOKABLE void startTimer(int, int);  // Метод для инициализации таймера
    Q_INVOKABLE void stopTimer();   // Метод для остановки таймера
    Q_INVOKABLE bool isTimerActive() const; // Активен ли таймер
    int timerValue() const;         // Метод для получения значения таймера

signals:
    void startTimerSignal(int, int);  // Сигнал для инициализации таймера внутри потока
    void timeoutSignal();
    void timerValueChanged();             // Сигнал для уведомления об изменении значения
    void timerStopped();

private slots:
    void onTimeout();  // Слот для обработки сигнала таймера
    void initializeTimer(int, int);  // Слот для инициализации таймера

private:
    QTimer *timer;
    int m_timerValue;  // Количество секунд, прошедших с момента старта
};

#endif // TIMERWORKER_H
