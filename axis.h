#ifndef AXIS_H
#define AXIS_H

#include "axisLine.h"
#include <QVector3D>
#include <QQuaternion>
#include <QObject>

class Axis : public QObject {
    Q_OBJECT

    // Делаем оси доступными как QML-свойства
    Q_PROPERTY(Line* xAxis READ getXAxis CONSTANT)
    Q_PROPERTY(Line* yAxis READ getYAxis CONSTANT)
    Q_PROPERTY(Line* zAxis READ getZAxis CONSTANT)

    // Q_PROPERTY(Line* xAxis READ getXAxis WRITE setXAxis NOTIFY axisChanged)
    // Q_PROPERTY(Line* yAxis READ getYAxis WRITE setYAxis NOTIFY axisChanged)
    // Q_PROPERTY(Line* zAxis READ getZAxis WRITE setZAxis NOTIFY axisChanged)

public:
    Axis();


    // Теперь возвращаем ссылки на объекты Line как Q_PROPERTY
    Q_INVOKABLE Line* getXAxis() const;
    Q_INVOKABLE Line* getYAxis() const;
    Q_INVOKABLE Line* getZAxis() const;

    Q_INVOKABLE void setXAxis(Line*);
    Q_INVOKABLE void setYAxis(Line*);
    Q_INVOKABLE void setZAxis(Line*);

    Q_INVOKABLE Axis* applyQuaternion(float, QVector3D);

    Q_INVOKABLE Axis* applyGivenQuaternion(float, float, float, float);

    Q_INVOKABLE Axis* applyEulerAnglesWithOrder(float, float, float, const QString&);

    Q_INVOKABLE Axis* eulerAngles(float, float, float, const QString&);

    // доп

    Q_INVOKABLE Axis* eulerXYZ(float, float, float);

    Q_INVOKABLE Axis* quaternionVector();

    Q_INVOKABLE Axis* changeQuatForAnimation(int, int, Axis*);

    Q_INVOKABLE void printCurrentQuaternion();

    Q_INVOKABLE QVector3D quatToEulerByHands(const QString& order, QQuaternion q);

    // конец доп

    Q_INVOKABLE Axis* applyAircraftAnglesGlobal(float, float, float, const QString&);

    Q_INVOKABLE Axis* aircraftAngles(float, float, float, const QString&);

    Q_INVOKABLE Axis* airAngles(float, float, float, const QString&);

    // Q_INVOKABLE QVector3D getEulerAnglesFromQuaternion(const QString&);

    // Q_INVOKABLE Axis* applyAircraftAnglesGlobal_1(float, float, float, const QString&);

    Q_INVOKABLE QVector3D quatToEul(const QString&);

    Q_INVOKABLE QVector3D quatToAir(const QString&);

    Q_INVOKABLE QVector3D quaternionToEulerAngles(const QString&);

    Q_INVOKABLE void initializeFirstLabels(int);

    Q_INVOKABLE Axis* initializeSecondLabels(const QVector3D&, int);

    Q_INVOKABLE void test();

    Q_INVOKABLE QVector3D quatToEulTest(const QString&, QQuaternion, int);

    Q_INVOKABLE QVector3D quatToAirTest(const QString&, QQuaternion, int);
signals:
    void axisChanged();

private:
    QVector3D createVector(double, double, double, const QString&);

    QVector3D rotateAroundX(const QVector3D&, float);
    QVector3D rotateAroundY(const QVector3D&, float);
    QVector3D rotateAroundZ(const QVector3D&, float);

    Line *xAxis, *yAxis, *zAxis;  // Оси X, Y, Z
    QQuaternion currentQuaernion;
    int quater;
};

#endif // AXIS_H
