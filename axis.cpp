#include "axis.h"
#include <QQuaternion>
#include <QtMath>  // Для работы с углами и тригонометрией
#include <QDebug>
#include <QThread>
#include <cmath>

#define FLT_EPSILON 0.000002
#define PI 3.14159265358979323846
#define TWO_PI 2 * PI
const double TARGET_ANGLE_POS = 12 * M_PI / 180.0; // 119 градусов в радианах
const double TARGET_ANGLE_NEG = -12 * M_PI / 180.0;

double  m_atan2(double y, double x) {
    double angle;

    // Проверка углов в зависимости от квадранта
    if (x > 0) {
        angle = std::atan(y / x);  // Первый и четвертый квадрант
        qDebug() << "1111111";
    } else if (x < 0 && y >= 0) {
        angle = std::atan(y / x) + PI;  // Второй квадрант
        qDebug() << "2222222";
    } else if (x < 0 && y < 0) {
        angle = std::atan(y / x) - PI;  // Третий квадрант
        qDebug() << "3333333";
    } else if (x == 0 && y > 0) {
        angle = PI / 2;
        qDebug() << "4444444";
    } else if (x == 0 && y < 0) {
        angle = -PI / 2;
        qDebug() << "5555555";
    } else {
        angle = 0;  // Для случая, когда x и y оба равны нулю
        qDebug() << "6666666";
    }

    // Корректируем угол, чтобы он находился в диапазоне от -π до π
    while (angle > PI) {
        angle -= TWO_PI;
    }

    while (angle < -PI) {
        angle += TWO_PI;
    }

    return angle;
}

int whatQuater(float& ang) {
    int flag0 = -1;

    // определяем четверть
    if (ang < 90 && ang >= 0 )
        flag0 = 1;
    else if (ang > -90 && ang <= 0)
        flag0 = 4;
    else if (ang < 180 && ang > 90)
        flag0 = 2;
    else if (ang < -90 && ang > -180)
        flag0 = 3;

    qDebug() << "QUATER FLAG = " << flag0;

    return flag0;
}

float reduceAng(float& ang) {
    while (ang > 180 || ang < -180) {
        qDebug() << "reduceAng |while| angles = " << ang;

        if (ang < -180) {
            qDebug() << "in cycle";
            ang += 360;
        }
        else if (ang > 180)
            ang -= 360;
    }

    // qDebug() << "reduceAng angles = " << ang;

    return ang;
}

QQuaternion quatByHands(float& ang0, float& ang1, float& ang2) {
    float ca, cb, ct, sa, sb, st;
    float qw, qx, qy, qz;
    ang0 = qDegreesToRadians(ang0);
    ang1 = qDegreesToRadians(ang1);
    ang2 = qDegreesToRadians(ang2);

    ca = cos(ang0/2);
    cb = cos(ang1/2);
    ct = cos(ang2/2);

    sa = sin(ang0/2);
    sb = sin(ang1/2);
    st = sin(ang2/2);

    // Для Эйлера, (q1*q2)*q3
    qw = ca*cb*ct - sa*sb*st;
    qx = sa*cb*ct + ca*sb*st;
    qy = ca*sb*ct - sa*cb*st;
    qz = ca*cb*st + sa*sb*ct;

    // Для самолетных, q3*(q2*q1)
    // qw = ca*cb*ct + sa*sb*st;
    // qx = sa*cb*ct - ca*sb*st;
    // qy = ca*sb*ct + sa*cb*st;
    // qz = ca*cb*st - sa*sb*ct;

    QQuaternion q (qw, qx, qy, qz);

    return q;
}

void Axis::setXAxis(Line* axis) {
    if (xAxis != axis) {
        xAxis = axis;
        emit axisChanged();  // Уведомляем, что ось изменилась
    }
}

void Axis::setYAxis(Line* axis) {
    if (yAxis != axis) {
        yAxis = axis;
        emit axisChanged();  // Уведомляем, что ось изменилась
    }
}

void Axis::setZAxis(Line* axis) {
    if (zAxis != axis) {
        zAxis = axis;
        emit axisChanged();  // Уведомляем, что ось изменилась
    }
}

Axis* Axis::eulerXYZ(float ang0, float ang1, float ang2) {
    ang0 = reduceAng(ang0);
    ang1 = reduceAng(ang1);
    ang2 = reduceAng(ang2);

    QQuaternion q0;
    QQuaternion q1;
    QQuaternion q2;

    QQuaternion finalQuaternion;


    finalQuaternion = quatByHands(ang0, ang1, ang2);
    // qDebug() << "Кватернион из эйлера: \n " << "scalar = " << finalQuaternion.scalar() << " x = " << finalQuaternion.x()
    //          << " y = " << finalQuaternion.y() << " z = " << finalQuaternion.z();

    currentQuaernion = finalQuaternion.normalized();
    // qDebug() << "Кватернион из current: \n " << "scalar = " << currentQuaernion.scalar() << " x = " << currentQuaernion.x()
    //          << " y = " << currentQuaernion.y() << " z = " << currentQuaernion.z();

    quater = whatQuater(ang1);

    Line *newXAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(xAxis->end()), QColor("red"));
    Line *newYAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(yAxis->end()), QColor("green"));
    Line *newZAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(zAxis->end()), QColor("blue"));

    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;
    newAxis->currentQuaernion = currentQuaernion;

    return newAxis;
}

QVector3D Axis::quatToEulerByHands(const QString& order, QQuaternion q) {
    float qw = q.scalar();
    float qx = q.x();
    float qy = q.y();
    float qz = q.z();

    float M_0_0 = 1 - 2*qy*qy - 2*qz*qz;
    float M_1_0 = 2*qx*qy + 2*qz*qw;
    float M_2_0 = 2*qx*qz - 2*qy*qw;

    float M_0_1 = 2*qx*qy - 2*qz*qw;
    float M_1_1 = 1 - 2*qx*qx - 2*qz*qz;
    float M_2_1 = 2*qy*qz + 2*qx*qw;

    float M_0_2 = 2*qx*qz + 2*qy*qw;
    float M_1_2 = 2*qy*qz - 2*qx*qw;
    float M_2_2 = 1 - 2*qx*qx - 2*qy*qy;
    std::array<std::array<float,3>,3> M = {0};

    M[0][0] = M_0_0; M[1][0] = M_0_1; M[2][0] = M_0_2;
    M[0][1] = M_1_0; M[1][1] = M_1_1; M[2][1] = M_1_2;
    M[0][2] = M_2_0; M[1][2] = M_2_1; M[2][2] = M_2_2;

    // Проходим по всем парам элементов матрицы
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                for (int l = 0; l < 3; ++l) {
                    // Вычисляем atan2 для элементов (-m0, m1)
                    double angle = atan2(M[i][j], M[k][l]);

                    // Проверяем, совпадает ли угол с 119 или -119 градусами
                    if (fabs(angle - TARGET_ANGLE_POS) < 1e-4 || fabs(angle - TARGET_ANGLE_NEG) < 1e-4) {
                        qDebug() << "Найдены элементы: m[" << i << "][" << j << "] и m[" << k << "][" << l << "]" ;
                        qDebug() << "Угол: " << angle * 180.0 / M_PI << " градусов";
                    }
                }
            }
        }
    }

    float ang0, ang1, ang2;

    if (order == "Y-Z-X") {
        ang0 = atan2(- M_2_0, M_0_0);
        ang1 = asin(M_1_0);
        ang2 = atan2(- M_1_2, M_1_1);

    } else if (order == "Y-X-Z") {
        ang0 = atan2(M_0_2, M_2_2);
        ang1 = -asin(M_1_2);
        ang2 = atan2(M_1_0, M_1_1);

    } else if (order == "X-Z-Y") {
        ang0 = - atan2(- M_2_1, M_1_1);
        ang1 = - asin(M_0_1);
        ang2 = - atan2(- M_0_2, M_0_0);

    } else if (order == "X-Y-Z") {
        ang0 = atan2(- M_1_2, M_2_2);
        ang1 = asin(M_0_2);
        ang2 = atan2(- M_0_1, M_0_0);

    } else if (order == "Z-Y-X") {
        ang0 = - atan2(- M_1_0, M_0_0);
        ang1 = - asin(M_2_0);
        ang2 = - atan2(- M_2_1, M_2_2);

    } else if (order == "Z-X-Y") {
        ang0 = atan2(- M_0_1, M_1_1);
        ang1 = asin(M_2_1);
        ang2 = atan2(- M_2_0, M_2_2);

    // --------------------------------------------------

    } else if (order == "X-Y-X") { // работает для ang1 от 0 до PI
        ang0 = atan2(M_1_0, -M_2_0);
        ang1 = acos(M_0_0);
        ang2 = atan2(M_0_1, M_0_2);

    }  else if (order == "X-Z-X") {
        ang0 = atan2(M_2_0, M_1_0);
        ang1 = acos(M_0_0);
        ang2 = atan2(M_0_2, -M_0_1);

    } else if (order == "Y-X-Y") {
        ang0 = atan2(M_0_1, M_2_1);
        ang1 = acos(M_1_1);
        ang2 = atan2(M_1_0, -M_1_2);

    } else if (order == "Y-Z-Y") {
        ang0 = atan2(M_2_1, -M_0_1);
        ang1 = acos(M_1_1);
        ang2 = atan2(M_1_2, M_1_0);

    } else if (order == "Z-X-Z") {
        ang0 = atan2(M_0_2, -M_1_2);
        ang1 = acos(M_2_2);
        ang2 = atan2(M_2_0, M_2_1);

    } else if (order == "Z-Y-Z") {  // ведеся работа
        ang0 = atan2(M_1_2, M_0_2);
        ang1 = acos(M_2_2);
        ang2 = atan2(M_2_1, -M_2_0);

    }

    return QVector3D(qRadiansToDegrees(ang0), qRadiansToDegrees(ang1), qRadiansToDegrees(ang2));

}

void Axis::printCurrentQuaternion() {
    qDebug() << "currentQuaternion: \n " << "scalar = " << currentQuaernion.scalar() << " x = " << currentQuaernion.x()
        << " y = " << currentQuaernion.y() << " z = " << currentQuaernion.z();
}

Axis* Axis::quaternionVector() {
    float x, y, z;

    x = currentQuaernion.x() * 100;
    y = currentQuaernion.y() * 100;
    z = currentQuaernion.z() * 100;

    qDebug() << "qX = " << x << " qY = " << y << " qZ = " << z;

    Line *qLineX = new Line(QVector3D(0, 0, 0), QVector3D(x, y, z), QColor("cyan"));
    Line *qLineY = new Line(QVector3D(0, 0, 0), QVector3D(0, 0, 0), QColor("cyan"));
    Line *qLineZ = new Line(QVector3D(0, 0, 0), QVector3D(0, 0, 0), QColor("cyan"));

    Axis* newAxis = new Axis();
    newAxis->xAxis = qLineX;
    newAxis->yAxis = qLineY;
    newAxis->zAxis = qLineZ;

    return newAxis;
}

Axis* Axis::changeQuatForAnimation(int allIterations, int iteration, Axis* axis) {
    QQuaternion q = currentQuaernion;
    float angle, angleNew, angleFinal;
    qDebug() << "allIterations = " << allIterations;
    qDebug() << "Iterations = " << iteration;


    angle = 2*acos(q.scalar());
    qDebug() << "Result = " << qRadiansToDegrees( angle ) / allIterations;
    qDebug() << "Angle = " << qRadiansToDegrees( angle );
    angleNew = qRadiansToDegrees( angle )/allIterations *(iteration);
    qDebug() << "AngleNew = " << angleNew;
    angleFinal = qDegreesToRadians(angleNew/2);

    QQuaternion qNew (cos(angleFinal), q.x()/sin(angle/2) * sin(angleFinal), q.y()/sin(angle/2) * sin(angleFinal), q.z()/sin(angle/2) * sin(angleFinal));
    qNew.normalize();

    qDebug() << "Первый кватернион: \n " << "scalar = " << q.scalar() << " x = " << q.x()
             << " y = " << q.y() << " z = " << q.z();

    qDebug() << "Текущий кватернион: \n " << "scalar = " << qNew.scalar() << " x = " << qNew.x()
             << " y = " << qNew.y() << " z = " << qNew.z();

    currentQuaernion = qNew;

    Line *newXAxis = new Line(QVector3D(0, 0, 0), qNew.rotatedVector(axis->xAxis->end()), QColor("red"));
    Line *newYAxis = new Line(QVector3D(0, 0, 0), qNew.rotatedVector(axis->yAxis->end()), QColor("green"));
    Line *newZAxis = new Line(QVector3D(0, 0, 0), qNew.rotatedVector(axis->zAxis->end()), QColor("blue"));

    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;

    return newAxis;
}

// Конструктор по умолчанию, создающий три оси
Axis::Axis() {
    xAxis = new Line(QVector3D(0, 0, 0), QVector3D(100, 0, 0), QColor("red"));
    yAxis = new Line(QVector3D(0, 0, 0), QVector3D(0, 100, 0), QColor("green"));
    zAxis = new Line(QVector3D(0, 0, 0), QVector3D(0, 0, 100), QColor("blue"));
}

void Axis::initializeFirstLabels(int flag) {
    if (flag == 0) {
        xAxis->setStart(QVector3D(98,2,0));
        xAxis->setEnd(QVector3D(100,6,0));

        yAxis->setStart(QVector3D(98,6,0));
        yAxis->setEnd(QVector3D(100,2,0));
        yAxis->setColor(QColor("red"));
    }

    if (flag == 1) {
        xAxis->setStart(QVector3D(2,92.5,0));
        xAxis->setEnd(QVector3D(6,99,0));
        xAxis->setColor(QColor("green"));

        yAxis->setStart(QVector3D(4,95,0));
        yAxis->setEnd(QVector3D(2,100,0));
    }

    if (flag == 2) {
        xAxis->setStart(QVector3D(0,6,97));
        xAxis->setEnd(QVector3D(0,6,100));
        xAxis->setColor(QColor("blue"));

        yAxis->setStart(QVector3D(0,6,97));
        yAxis->setEnd(QVector3D(0,2,100));
        yAxis->setColor(QColor("blue"));

        zAxis->setStart(QVector3D(0,2,97));
        zAxis->setEnd(QVector3D(0,2,100));
    }
}

Axis* Axis::initializeSecondLabels(const QVector3D& vec, int flag) {
    double x = vec.x();
    double y = vec.y();
    double z = vec.z();

    if (flag == 0 ) {
        Line *newXAxis = new Line(QVector3D(x+2, y+2, z+2), QVector3D(x+6, y+9, z+2), QColor("orange"));
        Line *newYAxis = new Line(QVector3D(x+6, y+2, z+2), QVector3D(x+2,y+7+2,z+2), QColor("orange"));
        Line *newZAxis = new Line(vec, vec, QColor("orange"));

        Axis* newAxis = new Axis();
        newAxis->xAxis = newXAxis;
        newAxis->yAxis = newYAxis;
        newAxis->zAxis = newZAxis;

    return newAxis;
    }

    if (flag == 1) {
        Line *newXAxis = new Line(QVector3D(x+2, y+2, z+2), QVector3D(x+4+2, y+7+2, z+2), QColor("brown"));
        Line *newYAxis = new Line(QVector3D(x+2, y+7+2, z+2), QVector3D(x+2+2,y+3+2,z+2), QColor("brown"));
        Line *newZAxis = new Line(vec, vec, QColor("brown"));

        Axis* newAxis = new Axis();
        newAxis->xAxis = newXAxis;
        newAxis->yAxis = newYAxis;
        newAxis->zAxis = newZAxis;

        return newAxis;
    }

    if (flag == 2) {

        Line *newXAxis = new Line(QVector3D(x+2, y+2, z+3+2), QVector3D(x+2, y+4+2, z+2), QColor("brown"));
        Line *newYAxis = new Line(QVector3D(x+2, y+2, z+2), QVector3D(x+2,y+2,z+3+2), QColor("brown"));
        Line *newZAxis = new Line(QVector3D(x+2, y+4+2, z+2), QVector3D(x+2, y+4+2, z+3+2), QColor("brown"));

        Axis* newAxis = new Axis();
        newAxis->xAxis = newXAxis;
        newAxis->yAxis = newYAxis;
        newAxis->zAxis = newZAxis;

        return newAxis;
    }

}

// Возвращаем ссылки на оси
Line* Axis::getXAxis() const {
    // qDebug() << "getXAxis called: Start" << xAxis->start() << "End" << xAxis->end();
    return xAxis;
}

Line* Axis::getYAxis() const {
    return yAxis;
}

Line* Axis::getZAxis() const {
    return zAxis;
}

Axis* Axis::applyGivenQuaternion(float w, float x, float y, float z) {
    QQuaternion quaternion(w, x, y, z);
    quaternion.normalize();

    // Применяем кватернион для вращения осей
    Line *newXAxis = new Line(QVector3D(0, 0, 0), quaternion.rotatedVector(xAxis->end()), QColor("red"));
    Line *newYAxis = new Line(QVector3D(0, 0, 0), quaternion.rotatedVector(yAxis->end()), QColor("green"));
    Line *newZAxis = new Line(QVector3D(0, 0, 0), quaternion.rotatedVector(zAxis->end()), QColor("blue"));

    // Создаем новый объект Axis с новыми осями
    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;

    return newAxis;
}

// Метод для применения кватернионного преобразования (остается без изменений)
Axis* Axis::applyQuaternion(float angle, QVector3D rotationAxis) {
    rotationAxis.normalize();
    float radAngle = qDegreesToRadians(angle);

    float sinHalfAngle = qSin(radAngle / 2.0);
    float cosHalfAngle = qCos(radAngle / 2.0);

    QQuaternion quaternion(
        cosHalfAngle,
        rotationAxis.x() * sinHalfAngle,
        rotationAxis.y() * sinHalfAngle,
        rotationAxis.z() * sinHalfAngle
        );


    Line *newXAxis = new Line(QVector3D(0, 0, 0), quaternion.rotatedVector(xAxis->end()), QColor("red"));
    Line *newYAxis = new Line(QVector3D(0, 0, 0), quaternion.rotatedVector(yAxis->end()), QColor("green"));
    Line *newZAxis = new Line(QVector3D(0, 0, 0), quaternion.rotatedVector(zAxis->end()), QColor("blue"));

    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;

    newAxis->currentQuaernion = quaternion;
    return newAxis;
}

QVector3D Axis::createVector(double x, double y, double z, const QString& order) {
    QVector3D result;

    if (order == "X-Y-Z") {
        result = QVector3D(x, y, z);  // Порядок X, Y, Z
    } else if (order == "X-Z-Y") {
        result = QVector3D(x, z, y);  // Порядок X, Z, Y
    } else if (order == "Y-Z-X") {
        result = QVector3D(z, x, y);  // Порядок Y, Z, X
    } else if (order == "Y-X-Z") {
        result = QVector3D(y, x, z);  // Порядок Y, X, Z

    } else if (order == "Z-X-Y") {
        result = QVector3D(y, z, x);  // Порядок Z, X, Y

    } else if (order == "Z-Y-X") {
        result = QVector3D(z, y, x);  // Порядок Z, Y, X
    } else if (order == "X-Y-X") {
        result = QVector3D(x, y, z);  // Порядок X, Y, X

    } else if (order == "X-Z-X") {
        result = QVector3D(x, z, y);  // Порядок X, Z, X

    } else if (order == "Y-X-Y") {
        result = QVector3D(y, x, z);  // Порядок Y, X, Y
    } else if (order == "Y-Z-Y") {
        result = QVector3D(x, z, y);  // Порядок Y, Z, Y

    } else if (order == "Z-X-Z") {
        result = QVector3D(y, x, z);  // Порядок Z, X, Z

    } else if (order == "Z-Y-Z") {
        result = QVector3D(x, y, z);  // Порядок Z, Y, Z
    } /*else {
        // Если передан неизвестный порядок, выводим предупреждение
        qWarning("Неверный порядок осей: %s", qPrintable(order));
        result = QVector3D(x, y, z);  // Возвращаем вектор в исходном порядке
    }*/

    qDebug() << "X =  " << x << "Y = " << y << "Z = " << z << result;
    qDebug() << "Преобразования " << result;

    return result;
}

void Axis::test() {
    float ang0, ang1, ang2;
    ang0 = -19;
    ang1 = 15;
    ang2 = -12;
    int flag0;

    // определяем четверть
    if (ang1 < 90 && ang1 >= 0 )
        flag0 = 1;
    else if (ang1 > -90 && ang1 <= 0)
        flag0 = 4;
    else if (ang1 < 180 && ang1 > 90)
        flag0 = 2;
    else if (ang1 < -90 && ang1 > -180)
        flag0 = 3;

    qDebug() << "FLAG = " << flag0;

    // QQuaternion qRoll  = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
    // QQuaternion qPitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang1);
    // QQuaternion qYaw   = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang2);

    QQuaternion q0  = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
    QQuaternion q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang1);
    QQuaternion q2   = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang2);

    QQuaternion q = q0 * q1 * q2; // создани кватерниона для Эйлера

    // QQuaternion q = qYaw * qRoll * qPitch;

    // QQuaternion q = qRoll * qYaw * qPitch; // создани кватерниона для Самолетных

    // QVector3D vec = quatToEulTest("Y-Z-X", q, flag0); // Эйлер x = -19, z = -122, y =33
    // QVector3D vec = quatToAirTest("X-Z-Y", q, flag0); // Самолетных

    QVector3D vec = quatToEulerByHands("Z-Y-Z", q);

    qDebug() << "Test input:" << "\n X = " << ang0 << "\n Y = " << ang1 << "\n Z = " << ang2 << "\n X = ";

    qDebug() << "Test output:" << "\n X = " << vec.x() << "\n Y = " << vec.y() << "\n Z = " << vec.z() << "\n X = ";

}

QVector3D Axis::quatToAirTest(const QString& order, QQuaternion q, int flag) {
    q.normalize();
    float Nq = q.x()*q.x() + q.y()*q.y() + q.z()*q.z() + q.scalar()*q.scalar();
    float s;

    if (Nq > 0)
        s = 2/Nq;
    else
        s = 0;


    float xs = q.x() * s;
    float ys = q.y() * s;
    float zs = q.z() * s;

    float wx = q.scalar() * xs;
    float wy = q.scalar() * ys;
    float wz = q.scalar() * zs;

    float xx = q.x() * xs;
    float xy = q.x() * ys;
    float xz = q.x() * zs;

    float yy = q.y() * ys;
    float yz = q.y() * zs;
    float zz = q.z() * zs;

    std::array<std::array<float,4>,4> M = {0};

    // Заполняем матрицу M
    M[0][0] = 1.0 - (yy + zz);
    M[0][1] = xy - wz;
    M[0][2] = xz + wy;
    M[0][3] = 0.0;

    M[1][0] = xy + wz;
    M[1][1] = 1.0 - (xx + zz);
    M[1][2] = yz - wx;
    M[1][3] = 0.0;

    M[2][0] = xz - wy;
    M[2][1] = yz + wx;
    M[2][2] = 1.0 - (xx + yy);
    M[2][3] = 0.0;

    M[3][0] = 0.0;
    M[3][1] = 0.0;
    M[3][2] = 0.0;
    M[3][3] = 1.0;

    float cy, sy;
    float gamma_rad, beta_rad, alpha_rad;

    if (order == "X-Y-Z") { // готово
        cy = sqrt(M[0][0] * M[0][0] + M[1][0] * M[1][0]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот Z-Y-X из Эйлера
            if (flag == 3 || flag == 2) {
                alpha_rad = -atan2(M[1][0], -M[0][0]); // ea.z
                gamma_rad = -atan2(M[2][1], -M[2][2]); // ea.x
            } else {
                alpha_rad = atan2(M[1][0], M[0][0]);
                gamma_rad = atan2(M[2][1], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[2][0], cy);      // ea.y
            else if (flag == 2)
                beta_rad = PI - atan2(-M[2][0], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[2][0], cy);

        } else {
            gamma_rad = atan2(-M[1][2], M[1][1]); // ea.x
            beta_rad = atan2(-M[2][0], cy);       // ea.y
            alpha_rad = 0;                        // ea.z
        }

    } else if (order == "X-Z-Y") { // готово
        cy = sqrt(M[0][0] * M[0][0] + M[2][0] * M[2][0]);
        if (cy > 16 * FLT_EPSILON) {
            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[1][2], -M[1][1]);
                alpha_rad = atan2(M[2][0], -M[0][0]);
            } else {
                gamma_rad = atan2(-M[1][2], M[1][1]);
                alpha_rad = atan2(-M[2][0], M[0][0]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = - atan2(-M[1][0], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[1][0], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[1][0], cy);
        } else {
            gamma_rad = -atan2(-M[2][1], M[2][2]);
            beta_rad = -atan2(-M[1][0], cy);
            alpha_rad = 0;
        }
    } else if (order == "Y-Z-X") { // готово
        cy = sqrt(M[1][1] * M[1][1] + M[2][1] * M[2][1]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = -atan2(M[0][2], -M[0][0]);
                alpha_rad = -atan2(M[2][1], -M[1][1]);
            } else {
                gamma_rad = atan2(M[0][2], M[0][0]);
                alpha_rad = atan2(M[2][1], M[1][1]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[0][1], cy);
            else if (flag == 2)
                beta_rad = PI - atan2(-M[0][1], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[0][1], cy);

        } else {
            gamma_rad = atan2(-M[2][0], M[2][2]);
            beta_rad = atan2(-M[0][1], cy);
            alpha_rad = 0;
        }
    } else if (order == "Y-X-Z") { // готово
        cy = sqrt(M[1][1] * M[1][1] + M[0][1] * M[0][1]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[2][0], -M[2][2]);
                alpha_rad = atan2(M[0][1], -M[1][1]);
            } else {
                gamma_rad = -atan2(M[2][0], M[2][2]);
                alpha_rad = -atan2(M[0][1], M[1][1]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = -atan2(-M[2][1], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[2][1], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[2][1], cy);

            // gamma_rad = -atan2(M[2][0], M[2][2]);
            // beta_rad = -atan2(-M[2][1], cy);
            // alpha_rad = -atan2(M[0][1], M[1][1]);
        } else {
            gamma_rad = -atan2(-M[0][2], M[0][0]);
            beta_rad = -atan2(-M[2][1], cy);
            alpha_rad = 0;
        }
    } else if (order == "Z-X-Y") { // готово
        cy = sqrt(M[2][2] * M[2][2] + M[0][2] * M[0][2]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = -atan2(M[1][0], -M[1][1]);
                alpha_rad = -atan2(M[0][2], -M[2][2]);
            } else {
                gamma_rad = atan2(M[1][0], M[1][1]);
                alpha_rad = atan2(M[0][2], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[1][2], cy);
            else if (flag == 2)
                beta_rad = PI - atan2(-M[1][2], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[1][2], cy);

        } else {
            gamma_rad = atan2(-M[0][1], M[0][0]);
            beta_rad = atan2(-M[1][2], cy);
            alpha_rad = 0;
        }
    } else if (order == "Z-Y-X") { // готово
        cy = sqrt(M[2][2] * M[2][2] + M[1][2] * M[1][2]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[0][1], -M[0][0]);
                alpha_rad = atan2(M[1][2], -M[2][2]);
            } else {
                gamma_rad = -atan2(M[0][1], M[0][0]);
                alpha_rad = -atan2(M[1][2], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = -atan2(-M[0][2], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[0][2], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[0][2], cy);
        } else {
            gamma_rad = -atan2(-M[1][0], M[1][1]);
            beta_rad = -atan2(-M[0][2], cy);
            alpha_rad = 0;
        }
    }

    return QVector3D(qRadiansToDegrees(gamma_rad), qRadiansToDegrees(beta_rad), qRadiansToDegrees(alpha_rad));
}

QVector3D Axis::quatToEulTest(const QString& order, QQuaternion q, int flag) {
    q.normalize();
    float Nq = q.x()*q.x() + q.y()*q.y() + q.z()*q.z() + q.scalar()*q.scalar();
    float s;

    if (Nq > 0)
        s = 2/Nq;
    else
        s = 0;


    float xs = q.x() * s;
    float ys = q.y() * s;
    float zs = q.z() * s;

    float wx = q.scalar() * xs;
    float wy = q.scalar() * ys;
    float wz = q.scalar() * zs;

    float xx = q.x() * xs;
    float xy = q.x() * ys;
    float xz = q.x() * zs;

    float yy = q.y() * ys;
    float yz = q.y() * zs;
    float zz = q.z() * zs;

    std::array<std::array<float,4>,4> M = {0};

    // Заполняем матрицу M
    M[0][0] = 1.0 - (yy + zz);
    M[0][1] = xy - wz;
    M[0][2] = xz + wy;
    M[0][3] = 0.0;

    M[1][0] = xy + wz;
    M[1][1] = 1.0 - (xx + zz);
    M[1][2] = yz - wx;
    M[1][3] = 0.0;

    M[2][0] = xz - wy;
    M[2][1] = yz + wx;
    M[2][2] = 1.0 - (xx + yy);
    M[2][3] = 0.0;

    M[3][0] = 0.0;
    M[3][1] = 0.0;
    M[3][2] = 0.0;
    M[3][3] = 1.0;

    float cy, sy;
    float gamma_rad, beta_rad, alpha_rad;

    if (order == "Z-Y-X") { // готово
        cy = sqrt(M[0][0] * M[0][0] + M[1][0] * M[1][0]);
        if (cy > 16 * FLT_EPSILON) {
            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                alpha_rad = -atan2(M[1][0], -M[0][0]); // ea.z
                gamma_rad = -atan2(M[2][1], -M[2][2]); // ea.x
            } else {
                alpha_rad = atan2(M[1][0], M[0][0]);
                gamma_rad = atan2(M[2][1], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[2][0], cy);      // ea.y
            else if (flag == 2)
                beta_rad = PI - atan2(-M[2][0], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[2][0], cy);


        } else {
            gamma_rad = atan2(-M[1][2], M[1][1]); // ea.x
            beta_rad = atan2(-M[2][0], cy);       // ea.y
            alpha_rad = 0;                        // ea.z
        }
    } else if (order == "X-Y-X") { // готово
        sy = sqrt(M[0][1] * M[0][1] + M[0][2] * M[0][2]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[0][1], -M[0][2]);
                alpha_rad = -atan2(M[1][0], M[2][0]);
            } else {
                gamma_rad = atan2(M[0][1], M[0][2]);
                alpha_rad = atan2(M[1][0], -M[2][0]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = - atan2(sy, M[0][0]);
            else
                beta_rad = atan2(sy, M[0][0]);

            // gamma_rad = -atan2(M[0][1], -M[0][2]);
            // beta_rad = atan2(sy, M[0][0]);
            // alpha_rad = -atan2(M[1][0], M[2][0]);
        } else {
            gamma_rad = atan2(-M[1][2], M[1][1]);
            beta_rad = atan2(sy, M[0][0]);
            alpha_rad = 0;
        }
    } else if (order == "Y-Z-X") { // готово
        cy = sqrt(M[0][0] * M[0][0] + M[2][0] * M[2][0]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[1][2], -M[1][1]);
                alpha_rad = atan2(M[2][0], -M[0][0]);
            } else {
                gamma_rad = atan2(-M[1][2], M[1][1]);
                alpha_rad = atan2(-M[2][0], M[0][0]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = - atan2(-M[1][0], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[1][0], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[1][0], cy);

            // gamma_rad = atan2(M[1][2], -M[1][1]);
            // beta_rad = -atan2(-M[1][0], cy);
            // alpha_rad = atan2(M[2][0], -M[0][0]);
            qDebug() << "QQQQQQQQQQQQQQQ";
        } else {
            gamma_rad = -atan2(-M[2][1], M[2][2]);
            beta_rad = -atan2(-M[1][0], cy);
            alpha_rad = 0;
            qDebug() << "qqqqqqqqqqqqqqqq";
        }
    } else if (order == "X-Z-X") { // готово
        sy = sqrt(M[0][2] * M[0][2] + M[0][1] * M[0][1]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[0][2], M[0][1]);
                alpha_rad = -atan2(M[2][0], -M[1][0]);
            } else {
                gamma_rad = atan2(M[0][2], -M[0][1]);
                alpha_rad = -atan2(-M[2][0], M[1][0]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = atan2(-sy, M[0][0]);
            else
                beta_rad = -atan2(-sy, M[0][0]);

            // gamma_rad = atan2(M[0][2], -M[0][1]);
            // beta_rad = -m_atan2(-sy, M[0][0]);
            // alpha_rad = atan2(M[2][0], M[1][0]);

            // qDebug() << "sy = " << sy << "   M[0][0] = " << M[0][0];
            // qDebug() << "atan = " << atan(-sy / M[0][0]);

            qDebug() << "M[0][2] = " << M[0][2] << "    M[0][1] = " << M[0][1];
            qDebug() << "atan = " << atan(M[0][2] / M[0][1]);
        } else {
            gamma_rad = -atan2(-M[2][1], M[2][2]);
            beta_rad = -atan2(sy, M[0][0]);
            alpha_rad = 0;
        }
    } else if (order == "X-Z-Y") { // готово
        cy = sqrt(M[1][1] * M[1][1] + M[2][1] * M[2][1]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = -atan2(M[0][2], -M[0][0]);
                alpha_rad = -atan2(M[2][1], -M[1][1]);
            } else {
                gamma_rad = atan2(M[0][2], M[0][0]);
                alpha_rad = atan2(M[2][1], M[1][1]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[0][1], cy);
            else if (flag == 2)
                beta_rad = PI - atan2(-M[0][1], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[0][1], cy);


            qDebug() << "ppppppppppp!";
        } else {
            gamma_rad = atan2(-M[2][0], M[2][2]);
            beta_rad = atan2(-M[0][1], cy);
            alpha_rad = 0;
            qDebug() << "oooooooooooo";
        }
    } else if (order == "Y-Z-Y") { // готово
        sy = sqrt(M[1][2] * M[1][2] + M[1][0] * M[1][0]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[1][2], -M[1][0]);
                alpha_rad = -atan2(M[2][1], M[0][1]);
            } else {
                gamma_rad = -atan2(-M[1][2], M[1][0]);
                alpha_rad = atan2(M[2][1], -M[0][1]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = -atan2(sy, M[1][1]);
            else
                beta_rad = atan2(sy, M[1][1]);

            // gamma_rad = -atan2(M[1][2], -M[1][0]);
            // beta_rad = atan2(sy, M[1][1]);
            // alpha_rad = -atan2(M[2][1], M[0][1]);
        } else {
            gamma_rad = atan2(-M[2][0], M[2][2]);
            beta_rad = atan2(sy, M[1][1]);
            alpha_rad = 0;
        }
    } else if (order == "Z-X-Y") { // готово
        cy = sqrt(M[1][1] * M[1][1] + M[0][1] * M[0][1]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[2][0], -M[2][2]);
                alpha_rad = atan2(M[0][1], -M[1][1]);
            } else {
                gamma_rad = -atan2(M[2][0], M[2][2]);
                alpha_rad = -atan2(M[0][1], M[1][1]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = -atan2(-M[2][1], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[2][1], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[2][1], cy);

            // gamma_rad = atan2(M[2][0], -M[2][2]);
            // beta_rad = -atan2(-M[2][1], cy);
            // alpha_rad = atan2(M[0][1], -M[1][1]);
        } else {
            gamma_rad = -atan2(-M[0][2], M[0][0]);
            beta_rad = -atan2(-M[2][1], cy);
            alpha_rad = 0;
        }
    } else if (order == "Y-X-Y") { // готово
        sy = sqrt(M[1][0] * M[1][0] + M[1][2] * M[1][2]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = atan2(-M[1][0], M[1][2]);
                alpha_rad = -atan2(M[0][1], -M[2][1]);
            } else {
                gamma_rad = atan2(M[1][0], -M[1][2]);
                alpha_rad = atan2(M[0][1], M[2][1]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = -atan2(sy, M[1][1]);
            else
                beta_rad = atan2(sy, M[1][1]);

            // gamma_rad = atan2(M[1][0], -M[1][2]);
            // beta_rad = -atan2(sy, M[1][1]);
            // beta_rad = acos(M[1][1]);
            // alpha_rad = atan2(M[0][1], M[2][1]);

            // qDebug() << "sy = " << sy << "   M[1][1] = " << M[1][1];
            // qDebug() << "atan = " << atan(sy / M[1][1]);

            // qDebug() << "M[1][0] = " << M[1][0] << "    M[1][2] = " << M[1][2];
            // qDebug() << "atan = " << atan(M[1][0] / M[1][2]);
        } else {
            gamma_rad = -atan2(-M[0][2], M[0][0]);
            beta_rad = -atan2(sy, M[1][1]);
            alpha_rad = 0;
        }
    } else if (order == "Y-X-Z") { // готово
        cy = sqrt(M[2][2] * M[2][2] + M[0][2] * M[0][2]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = -atan2(M[1][0], -M[1][1]);
                alpha_rad = -atan2(M[0][2], -M[2][2]);
            } else {
                gamma_rad = atan2(M[1][0], M[1][1]);
                alpha_rad = atan2(M[0][2], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[1][2], cy);
            else if (flag == 2)
                beta_rad = PI - atan2(-M[1][2], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[1][2], cy);

            // gamma_rad = -atan2(M[1][0], -M[1][1]);
            // beta_rad = atan2(-M[1][2], cy);
            // alpha_rad = -atan2(M[0][2], -M[2][2]);
        } else {
            gamma_rad = atan2(-M[0][1], M[0][0]);
            beta_rad = atan2(-M[1][2], cy);
            alpha_rad = 0;
        }
    } else if (order == "Z-X-Z") { // готово
        sy = sqrt(M[2][0] * M[2][0] + M[2][1] * M[2][1]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[2][0], -M[2][1]);
                alpha_rad = -atan2(M[0][2], M[1][2]);
            } else {
                gamma_rad = atan2(M[2][0], M[2][1]);
                alpha_rad = atan2(M[0][2], -M[1][2]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = -atan2(sy, M[2][2]);
            else
                beta_rad = atan2(sy, M[2][2]);

            // gamma_rad = -atan2(M[2][0], -M[2][1]);
            // beta_rad = atan2(sy, M[2][2]);
            // alpha_rad = -atan2(M[0][2], M[1][2]);
        } else {
            gamma_rad = atan2(-M[0][1], M[0][0]);
            beta_rad = atan2(sy, M[2][2]);
            alpha_rad = 0;
        }
    } else if (order == "X-Y-Z") { // готово
        cy = sqrt(M[2][2] * M[2][2] + M[1][2] * M[1][2]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[0][1], -M[0][0]);
                alpha_rad = atan2(M[1][2], -M[2][2]);
            } else {
                gamma_rad = -atan2(M[0][1], M[0][0]);
                alpha_rad = -atan2(M[1][2], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = -atan2(-M[0][2], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[0][2], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[0][2], cy);

            // gamma_rad = atan2(M[0][1], -M[0][0]);
            // beta_rad = -atan2(-M[0][2], cy);
            // alpha_rad = atan2(M[1][2], -M[2][2]);
        } else {
            gamma_rad = -atan2(-M[1][0], M[1][1]);
            beta_rad = -atan2(-M[0][2], cy);
            alpha_rad = 0;
        }
    } else if (order == "Z-Y-Z") { // готово
        sy = sqrt(M[2][1] * M[2][1] + M[2][0] * M[2][0]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[2][1], M[2][0]);
                alpha_rad = -atan2(M[1][2], -M[0][2]);
            } else {
                gamma_rad = atan2(M[2][1], -M[2][0]);
                alpha_rad = -atan2(-M[1][2], M[0][2]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad  = -atan2(sy, M[2][2]);
            else
                beta_rad  = atan2(sy, M[2][2]);


            // if (M[2][0] < 0 && M[2][1] > 0) {
            //     gamma_rad = -atan2(M[2][1], M[2][0]);
            // } else {
            //     gamma_rad = atan2(-M[2][1], M[2][0]);
            // }

            // beta_rad  = -atan2(sy, M[2][2]);
            // beta_rad = acosf(M[2][2]);
            // alpha_rad = -atan2(M[1][2], -M[0][2]);

            // qDebug() << "lllll" << "\n";
            // qDebug() << "M[1][2] = " << M[1][2] << "    M[0][2] = " << M[0][2];
            // qDebug() << "atan = " << atan(M[1][2] / M[0][2]);
        } else {
            gamma_rad = -atan2(-M[1][0], M[1][1]);
            beta_rad = -atan2(sy, M[2][2]);
            alpha_rad = 0;
            qDebug() << "fffffff" << "\n";
        }
    }

    return QVector3D(qRadiansToDegrees(alpha_rad), qRadiansToDegrees(beta_rad), qRadiansToDegrees(gamma_rad));
}

Axis* Axis::eulerAngles(float ang0, float ang1, float ang2, const QString& order) {
    ang0 = reduceAng(ang0);
    ang1 = reduceAng(ang1);
    ang2 = reduceAng(ang2);

    QQuaternion q0;
    QQuaternion q1;
    QQuaternion q2;

    QQuaternion finalQuaternion;
    // Порядок введения
    if (order == "X-Y-Z") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang2);
    } else if (order == "X-Z-Y") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang2);
    } else if (order == "Y-Z-X") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang2);
    } else if (order == "Y-X-Z") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang2);
    } else if (order == "Z-X-Y") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang2);
    } else if (order == "Z-Y-X") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang2);
    } else if (order == "Y-Z-Y") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang2);
    } else if (order == "Y-X-Y") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang2);
    } else if (order == "Z-X-Z") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang2);
    } else if (order == "Z-Y-Z") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang2);
    } else if (order == "X-Z-X") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang2);
    } else if (order == "X-Y-X") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang2);
    }

    finalQuaternion = q0*q1*q2;


    // тестовая часть
    QQuaternion qByHands;
    if (order == "X-Y-Z") {
        qByHands = quatByHands(ang0, ang1, ang2);
        qByHands.normalize();
        qDebug() << "FFFFFFF";
    }
    qDebug() << " fqw = " << finalQuaternion.scalar() << "\n fqx = " << finalQuaternion.x() << "\n fqy = " << finalQuaternion.y()
             << "\n fqz = " << finalQuaternion.z();

    qDebug() << "";

    qDebug() << " qw = " << qByHands.scalar() << "\n qx = " << qByHands.x() << "\n qy = " << qByHands.y()
             << "\n qz = " << qByHands.z();

    // конец тестовой части

    currentQuaernion = finalQuaternion.normalized();
    quater = whatQuater(ang1);

    Line *newXAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(xAxis->end()), QColor("red"));
    Line *newYAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(yAxis->end()), QColor("green"));
    Line *newZAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(zAxis->end()), QColor("blue"));

    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;

    return newAxis;
}

Axis* Axis::applyEulerAnglesWithOrder(float ang0, float ang1, float ang2, const QString& order) {
    // Уопрядочиваем углы в правльную последовательность для работы
    QVector3D vec = createVector(ang0, ang1, ang2, order);
    qDebug() << "углы: \n" << "1 = " << ang0 << "\n" << "2 = " << ang1 << "\n" << "3 = " << ang2 << "\n";

    // Кватернионы для осей
    QQuaternion qRoll = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), vec.x());
    QQuaternion qPitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), vec.y());
    QQuaternion qYaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), vec.z());

    QQuaternion finalQuaternion;
    // Порядок введения
    if (order == "X-Y-Z") {
        finalQuaternion = qRoll * qPitch * qYaw;
    } else if (order == "X-Z-Y") {
        finalQuaternion = qRoll * qYaw * qPitch;
    } else if (order == "Y-Z-X") {
        finalQuaternion = qPitch * qYaw * qRoll;
    } else if (order == "Y-X-Z") {
        finalQuaternion = qPitch * qRoll * qYaw;
    } else if (order == "Z-X-Y") {
        int ang0 = vec.z();
        int ang1 = vec.x();
        int ang2 = vec.y();
        qRoll  = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
        qPitch = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang1);
        qYaw   = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang2);

        finalQuaternion = qRoll * qPitch * qYaw;
        // finalQuaternion = qYaw * qRoll * qPitch;
    } else if (order == "Z-Y-X") {
        finalQuaternion = qYaw * qPitch * qRoll;

    } else if (order == "Y-Z-Y") {
        qRoll = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), vec.x());
        finalQuaternion = qRoll * qYaw * qPitch;
    } else if (order == "Y-X-Y") {
        qYaw = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), vec.z());
        finalQuaternion = qPitch * qRoll * qYaw;

    } else if (order == "Z-X-Z") {
        qPitch = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), vec.y());
        finalQuaternion = qPitch * qRoll * qYaw;
    } else if (order == "Z-Y-Z") {
        qRoll = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), vec.x());
        finalQuaternion = qRoll * qPitch * qYaw;

    } else if (order == "X-Z-X") {
        qPitch = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), vec.y());
        finalQuaternion = qRoll * qYaw * qPitch;
    } else if (order == "X-Y-X") {
        qYaw = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), vec.z());
        finalQuaternion = qRoll * qPitch * qYaw;
    }

    currentQuaernion = finalQuaternion;

    Line *newXAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(xAxis->end()), QColor("red"));
    Line *newYAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(yAxis->end()), QColor("green"));
    Line *newZAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(zAxis->end()), QColor("blue"));

    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;

    return newAxis;
}

QVector3D Axis::quatToEul(const QString& order) { // не рабочее
    QQuaternion q = currentQuaernion.normalized();
    int flag = quater;
    float Nq = q.x()*q.x() + q.y()*q.y() + q.z()*q.z() + q.scalar()*q.scalar();
    float s;

    if (Nq > 0)
        s = 2/Nq;
    else
        s = 0;


    float xs = q.x() * s;
    float ys = q.y() * s;
    float zs = q.z() * s;

    float wx = q.scalar() * xs;
    float wy = q.scalar() * ys;
    float wz = q.scalar() * zs;

    float xx = q.x() * xs;
    float xy = q.x() * ys;
    float xz = q.x() * zs;

    float yy = q.y() * ys;
    float yz = q.y() * zs;
    float zz = q.z() * zs;

    std::array<std::array<float,4>,4> M = {0};

    // Заполняем матрицу M
    M[0][0] = 1.0 - (yy + zz);
    M[0][1] = xy - wz;
    M[0][2] = xz + wy;
    M[0][3] = 0.0;

    M[1][0] = xy + wz;
    M[1][1] = 1.0 - (xx + zz);
    M[1][2] = yz - wx;
    M[1][3] = 0.0;

    M[2][0] = xz - wy;
    M[2][1] = yz + wx;
    M[2][2] = 1.0 - (xx + yy);
    M[2][3] = 0.0;

    M[3][0] = 0.0;
    M[3][1] = 0.0;
    M[3][2] = 0.0;
    M[3][3] = 1.0;

    float cy, sy;
    float gamma_rad, beta_rad, alpha_rad;

    if (order == "Z-Y-X") { // готово
        cy = sqrt(M[0][0] * M[0][0] + M[1][0] * M[1][0]);
        if (cy > 16 * FLT_EPSILON) {
            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                alpha_rad = -atan2(M[1][0], -M[0][0]); // ea.z
                gamma_rad = -atan2(M[2][1], -M[2][2]); // ea.x
            } else {
                alpha_rad = atan2(M[1][0], M[0][0]);
                gamma_rad = atan2(M[2][1], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[2][0], cy);      // ea.y
            else if (flag == 2)
                beta_rad = PI - atan2(-M[2][0], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[2][0], cy);


        } else {
            gamma_rad = atan2(-M[1][2], M[1][1]); // ea.x
            beta_rad = atan2(-M[2][0], cy);       // ea.y
            alpha_rad = 0;                        // ea.z
        }
    } else if (order == "X-Y-X") { // готово
        sy = sqrt(M[0][1] * M[0][1] + M[0][2] * M[0][2]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[0][1], -M[0][2]);
                alpha_rad = -atan2(M[1][0], M[2][0]);
            } else {
                gamma_rad = atan2(M[0][1], M[0][2]);
                alpha_rad = atan2(M[1][0], -M[2][0]);
            }

            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = - atan2(sy, M[0][0]);
            else
                beta_rad = atan2(sy, M[0][0]);

        } else {
            gamma_rad = atan2(-M[1][2], M[1][1]);
            beta_rad = atan2(sy, M[0][0]);
            alpha_rad = 0;
        }
    } else if (order == "Y-Z-X") { // готово
        cy = sqrt(M[0][0] * M[0][0] + M[2][0] * M[2][0]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[1][2], -M[1][1]);
                alpha_rad = atan2(M[2][0], -M[0][0]);
            } else {
                gamma_rad = atan2(-M[1][2], M[1][1]);
                alpha_rad = atan2(-M[2][0], M[0][0]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = - atan2(-M[1][0], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[1][0], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[1][0], cy);

        } else {
            gamma_rad = -atan2(-M[2][1], M[2][2]);
            beta_rad = -atan2(-M[1][0], cy);
            alpha_rad = 0;
        }

    } else if (order == "X-Z-X") { // готово
        sy = sqrt(M[0][2] * M[0][2] + M[0][1] * M[0][1]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[0][2], M[0][1]);
                alpha_rad = -atan2(M[2][0], -M[1][0]);
            } else {
                gamma_rad = atan2(M[0][2], -M[0][1]);
                alpha_rad = -atan2(-M[2][0], M[1][0]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = atan2(-sy, M[0][0]);
            else
                beta_rad = -atan2(-sy, M[0][0]);

        } else {
            gamma_rad = -atan2(-M[2][1], M[2][2]);
            beta_rad = -atan2(sy, M[0][0]);
            alpha_rad = 0;
        }
    } else if (order == "X-Z-Y") { // готово
        cy = sqrt(M[1][1] * M[1][1] + M[2][1] * M[2][1]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = -atan2(M[0][2], -M[0][0]);
                alpha_rad = -atan2(M[2][1], -M[1][1]);
            } else {
                gamma_rad = atan2(M[0][2], M[0][0]);
                alpha_rad = atan2(M[2][1], M[1][1]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[0][1], cy);
            else if (flag == 2)
                beta_rad = PI - atan2(-M[0][1], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[0][1], cy);

        } else {
            gamma_rad = atan2(-M[2][0], M[2][2]);
            beta_rad = atan2(-M[0][1], cy);
            alpha_rad = 0;
        }

    } else if (order == "Y-Z-Y") { // готово
        sy = sqrt(M[1][2] * M[1][2] + M[1][0] * M[1][0]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[1][2], -M[1][0]);
                alpha_rad = -atan2(M[2][1], M[0][1]);
            } else {
                gamma_rad = -atan2(-M[1][2], M[1][0]);
                alpha_rad = atan2(M[2][1], -M[0][1]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = -atan2(sy, M[1][1]);
            else
                beta_rad = atan2(sy, M[1][1]);

        } else {
            gamma_rad = atan2(-M[2][0], M[2][2]);
            beta_rad = atan2(sy, M[1][1]);
            alpha_rad = 0;
        }
    } else if (order == "Z-X-Y") { // готово
        cy = sqrt(M[1][1] * M[1][1] + M[0][1] * M[0][1]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[2][0], -M[2][2]);
                alpha_rad = atan2(M[0][1], -M[1][1]);
            } else {
                gamma_rad = -atan2(M[2][0], M[2][2]);
                alpha_rad = -atan2(M[0][1], M[1][1]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = -atan2(-M[2][1], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[2][1], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[2][1], cy);

        } else {
            gamma_rad = -atan2(-M[0][2], M[0][0]);
            beta_rad = -atan2(-M[2][1], cy);
            alpha_rad = 0;
        }
    } else if (order == "Y-X-Y") { // готово
        sy = sqrt(M[1][0] * M[1][0] + M[1][2] * M[1][2]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = atan2(-M[1][0], M[1][2]);
                alpha_rad = -atan2(M[0][1], -M[2][1]);
            } else {
                gamma_rad = atan2(M[1][0], -M[1][2]);
                alpha_rad = atan2(M[0][1], M[2][1]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = -atan2(sy, M[1][1]);
            else
                beta_rad = atan2(sy, M[1][1]);

        } else {
            gamma_rad = -atan2(-M[0][2], M[0][0]);
            beta_rad = -atan2(sy, M[1][1]);
            alpha_rad = 0;
        }
    } else if (order == "Y-X-Z") { // готово
        cy = sqrt(M[2][2] * M[2][2] + M[0][2] * M[0][2]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = -atan2(M[1][0], -M[1][1]);
                alpha_rad = -atan2(M[0][2], -M[2][2]);
            } else {
                gamma_rad = atan2(M[1][0], M[1][1]);
                alpha_rad = atan2(M[0][2], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[1][2], cy);
            else if (flag == 2)
                beta_rad = PI - atan2(-M[1][2], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[1][2], cy);

        } else {
            gamma_rad = atan2(-M[0][1], M[0][0]);
            beta_rad = atan2(-M[1][2], cy);
            alpha_rad = 0;
        }
    } else if (order == "Z-X-Z") { // готово
        sy = sqrt(M[2][0] * M[2][0] + M[2][1] * M[2][1]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[2][0], -M[2][1]);
                alpha_rad = -atan2(M[0][2], M[1][2]);
            } else {
                gamma_rad = atan2(M[2][0], M[2][1]);
                alpha_rad = atan2(M[0][2], -M[1][2]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad = -atan2(sy, M[2][2]);
            else
                beta_rad = atan2(sy, M[2][2]);

        } else {
            gamma_rad = atan2(-M[0][1], M[0][0]);
            beta_rad = atan2(sy, M[2][2]);
            alpha_rad = 0;
        }
    } else if (order == "X-Y-Z") { // готово
        cy = sqrt(M[2][2] * M[2][2] + M[1][2] * M[1][2]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[0][1], -M[0][0]);
                alpha_rad = atan2(M[1][2], -M[2][2]);
            } else {
                gamma_rad = -atan2(M[0][1], M[0][0]);
                alpha_rad = -atan2(M[1][2], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = -atan2(-M[0][2], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[0][2], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[0][2], cy);

        } else {
            gamma_rad = -atan2(-M[1][0], M[1][1]);
            beta_rad = -atan2(-M[0][2], cy);
            alpha_rad = 0;
        }
    } else if (order == "Z-Y-Z") { // готово
        sy = sqrt(M[2][1] * M[2][1] + M[2][0] * M[2][0]);
        if (sy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 4) {
                gamma_rad = -atan2(M[2][1], M[2][0]);
                alpha_rad = -atan2(M[1][2], -M[0][2]);
            } else {
                gamma_rad = atan2(M[2][1], -M[2][0]);
                alpha_rad = -atan2(-M[1][2], M[0][2]);
            }
            // второй поворот
            if (flag == 3 || flag == 4)
                beta_rad  = -atan2(sy, M[2][2]);
            else
                beta_rad  = atan2(sy, M[2][2]);

        } else {
            gamma_rad = -atan2(-M[1][0], M[1][1]);
            beta_rad = -atan2(sy, M[2][2]);
            alpha_rad = 0;
        }
    }

    return QVector3D(qRadiansToDegrees(alpha_rad), qRadiansToDegrees(beta_rad), qRadiansToDegrees(gamma_rad));
}

QVector3D Axis::quatToAir(const QString& order) {
    QQuaternion q = currentQuaernion.normalized();
    int flag = quater;
    float Nq = q.x()*q.x() + q.y()*q.y() + q.z()*q.z() + q.scalar()*q.scalar();
    float s;

    if (Nq > 0)
        s = 2/Nq;
    else
        s = 0;


    float xs = q.x() * s;
    float ys = q.y() * s;
    float zs = q.z() * s;

    float wx = q.scalar() * xs;
    float wy = q.scalar() * ys;
    float wz = q.scalar() * zs;

    float xx = q.x() * xs;
    float xy = q.x() * ys;
    float xz = q.x() * zs;

    float yy = q.y() * ys;
    float yz = q.y() * zs;
    float zz = q.z() * zs;

    std::array<std::array<float,4>,4> M = {0};

    // Заполняем матрицу M
    M[0][0] = 1.0 - (yy + zz);
    M[0][1] = xy - wz;
    M[0][2] = xz + wy;
    M[0][3] = 0.0;

    M[1][0] = xy + wz;
    M[1][1] = 1.0 - (xx + zz);
    M[1][2] = yz - wx;
    M[1][3] = 0.0;

    M[2][0] = xz - wy;
    M[2][1] = yz + wx;
    M[2][2] = 1.0 - (xx + yy);
    M[2][3] = 0.0;

    M[3][0] = 0.0;
    M[3][1] = 0.0;
    M[3][2] = 0.0;
    M[3][3] = 1.0;

    float cy, sy;
    float gamma_rad, beta_rad, alpha_rad;

    if (order == "X-Y-Z") { // готово
        cy = sqrt(M[0][0] * M[0][0] + M[1][0] * M[1][0]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот Z-Y-X из Эйлера
            if (flag == 3 || flag == 2) {
                alpha_rad = -atan2(M[1][0], -M[0][0]); // ea.z
                gamma_rad = -atan2(M[2][1], -M[2][2]); // ea.x
            } else {
                alpha_rad = atan2(M[1][0], M[0][0]);
                gamma_rad = atan2(M[2][1], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[2][0], cy);      // ea.y
            else if (flag == 2)
                beta_rad = PI - atan2(-M[2][0], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[2][0], cy);

        } else {
            gamma_rad = atan2(-M[1][2], M[1][1]); // ea.x
            beta_rad = atan2(-M[2][0], cy);       // ea.y
            alpha_rad = 0;                        // ea.z
        }

    } else if (order == "X-Z-Y") { // готово
        cy = sqrt(M[0][0] * M[0][0] + M[2][0] * M[2][0]);
        if (cy > 16 * FLT_EPSILON) {
            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[1][2], -M[1][1]);
                alpha_rad = atan2(M[2][0], -M[0][0]);
            } else {
                gamma_rad = atan2(-M[1][2], M[1][1]);
                alpha_rad = atan2(-M[2][0], M[0][0]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = - atan2(-M[1][0], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[1][0], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[1][0], cy);
        } else {
            gamma_rad = -atan2(-M[2][1], M[2][2]);
            beta_rad = -atan2(-M[1][0], cy);
            alpha_rad = 0;
        }
    } else if (order == "Y-Z-X") { // готово
        cy = sqrt(M[1][1] * M[1][1] + M[2][1] * M[2][1]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = -atan2(M[0][2], -M[0][0]);
                alpha_rad = -atan2(M[2][1], -M[1][1]);
            } else {
                gamma_rad = atan2(M[0][2], M[0][0]);
                alpha_rad = atan2(M[2][1], M[1][1]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[0][1], cy);
            else if (flag == 2)
                beta_rad = PI - atan2(-M[0][1], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[0][1], cy);

        } else {
            gamma_rad = atan2(-M[2][0], M[2][2]);
            beta_rad = atan2(-M[0][1], cy);
            alpha_rad = 0;
        }
    } else if (order == "Y-X-Z") { // готово
        cy = sqrt(M[1][1] * M[1][1] + M[0][1] * M[0][1]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[2][0], -M[2][2]);
                alpha_rad = atan2(M[0][1], -M[1][1]);
            } else {
                gamma_rad = -atan2(M[2][0], M[2][2]);
                alpha_rad = -atan2(M[0][1], M[1][1]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = -atan2(-M[2][1], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[2][1], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[2][1], cy);

            // gamma_rad = -atan2(M[2][0], M[2][2]);
            // beta_rad = -atan2(-M[2][1], cy);
            // alpha_rad = -atan2(M[0][1], M[1][1]);
        } else {
            gamma_rad = -atan2(-M[0][2], M[0][0]);
            beta_rad = -atan2(-M[2][1], cy);
            alpha_rad = 0;
        }
    } else if (order == "Z-X-Y") { // готово
        cy = sqrt(M[2][2] * M[2][2] + M[0][2] * M[0][2]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = -atan2(M[1][0], -M[1][1]);
                alpha_rad = -atan2(M[0][2], -M[2][2]);
            } else {
                gamma_rad = atan2(M[1][0], M[1][1]);
                alpha_rad = atan2(M[0][2], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = atan2(-M[1][2], cy);
            else if (flag == 2)
                beta_rad = PI - atan2(-M[1][2], cy);
            else if (flag == 3)
                beta_rad = - PI - atan2(-M[1][2], cy);

        } else {
            gamma_rad = atan2(-M[0][1], M[0][0]);
            beta_rad = atan2(-M[1][2], cy);
            alpha_rad = 0;
        }
    } else if (order == "Z-Y-X") { // готово
        cy = sqrt(M[2][2] * M[2][2] + M[1][2] * M[1][2]);
        if (cy > 16 * FLT_EPSILON) {

            // первый и третий поворот
            if (flag == 3 || flag == 2) {
                gamma_rad = atan2(M[0][1], -M[0][0]);
                alpha_rad = atan2(M[1][2], -M[2][2]);
            } else {
                gamma_rad = -atan2(M[0][1], M[0][0]);
                alpha_rad = -atan2(M[1][2], M[2][2]);
            }

            // второй поворот
            if (flag == 1 || flag == 4)
                beta_rad = -atan2(-M[0][2], cy);
            else if (flag == 2)
                beta_rad = PI + atan2(-M[0][2], cy);
            else if (flag == 3)
                beta_rad = - PI + atan2(-M[0][2], cy);
        } else {
            gamma_rad = -atan2(-M[1][0], M[1][1]);
            beta_rad = -atan2(-M[0][2], cy);
            alpha_rad = 0;
        }
    }

    return QVector3D(qRadiansToDegrees(gamma_rad), qRadiansToDegrees(beta_rad), qRadiansToDegrees(alpha_rad));
}

QVector3D Axis::quaternionToEulerAngles(const QString& order) {
    QQuaternion q = currentQuaernion.normalized();
    float roll = 1, pitch = 2, yaw = 3;
    // float s, nq;
    // nq = q.x()*q.x() + q.y()*q.y() + q.z()*q.z() + q.scalar()*q.scalar();
    // if (nq > 0)
    //     s = 2/nq;
    // else
    //     s = 0;

    // есть функция toEulerAngles которая возвращает QVector3D и берет это от кватерниона.

    // if (order == "X-Z-Y") {
    if (order == "Y-Z-X") {
        roll = std::atan2(2 * (q.scalar() * q.z() + q.x() * q.y()), 1 - 2 * (q.y() * q.y() + q.z() * q.z()));
        pitch = std::asin(2 * (q.scalar() * q.x() - q.y() * q.z()));
        yaw = std::atan2(2 * (q.scalar() * q.x() + q.y() * q.z()), 1 - 2 * (q.x() * q.x() + q.z() * q.z()));
    }
    if (order == "Y-X-Z") {
        roll = std::atan2(2 * (q.scalar() * q.y() + q.x() * q.z()), 1 - 2 * (q.y() * q.y() + q.z() * q.z()));
        pitch = std::asin(2 * (q.scalar() * q.z() + q.x() * q.y()));
        yaw = std::atan2(2 * (q.scalar() * q.z() + q.y() * q.x()), 1 - 2 * (q.x() * q.x() + q.z() * q.z()));
    }


    // if (order == "Y-Z-X") {
    if (order == "X-Z-Y") {
        roll = std::atan2(2 * (q.scalar() * q.y() + q.z() * q.x()), 1 - 2 * (q.y() * q.y() + q.z() * q.z()));
        pitch = std::asin(2 * (-q.scalar() * q.z() + q.y() * q.x()));
        yaw = std::atan2(2 * (q.scalar() * q.x() + q.z() * q.y()), 1 - 2 * (q.x() * q.x() + q.y() * q.y()));
    }


    // if (order == "Z-X-Y") {
    //     roll = std::atan2(2 * (q.scalar() * q.z() + q.x() * q.y()), 1 - 2 * (q.z() * q.z() + q.y() * q.y()));
    //     pitch = std::asin(2 * (q.scalar() * q.x() - q.z() * q.y()));
    //     yaw = std::atan2(2 * (q.scalar() * q.y() + q.z() * q.x()), 1 - 2 * (q.y() * q.y() + q.x() * q.x()));
    // }
    // НЕ РАБОТАЕТ
    if (order == "Z-X-Y") {
        float cy = sqrt( (1.0-(q.x()*q.x() + q.z()*q.z()) ));
        roll = - std::atan2(2 * (- q.scalar() * q.y() + q.x() * q.z()), 1 - 2 * (q.x() * q.x() + q.y() * q.y()));
        pitch = std::asin(2 * (q.scalar() * q.z() - q.y() * q.x()));
        yaw = std::atan2(2 * (q.scalar() * q.z() + q.x() * q.y()), 1 - 2 * (q.z() * q.z() + q.y() * q.y()));
    }

    // РАБОТАЕТ
    if (order == "Z-Y-X") {
        roll = std::atan2(2 * (q.scalar() * q.x() + q.y() * q.z()), 1 - 2 * (q.x() * q.x() + q.y() * q.y()));
        pitch = std::asin(2 * (q.scalar() * q.y() - q.z() * q.x()));
        yaw = std::atan2(2 * (q.scalar() * q.z() + q.x() * q.y()), 1 - 2 * (q.y() * q.y() + q.z() * q.z()));
    }

    else if (order == "X-Z-X") {
        roll = std::atan2(2 * (q.scalar() * q.x() + q.z() * q.y()), 1 - 2 * (q.x() * q.x() + q.z() * q.z()));
        pitch = std::atan2(2 * (q.scalar() * q.z() + q.x() * q.y()), 1 - 2 * (q.y() * q.y() + q.z() * q.z()));
        yaw = std::asin(2 * (q.scalar() * q.x() - q.y() * q.z()));
    }
    if (order == "X-Y-X") {
        roll = std::atan2(2 * (q.scalar() * q.x() + q.y() * q.z()), 1 - 2 * (q.x() * q.x() + q.y() * q.y()));
        pitch = std::atan2(2 * (q.scalar() * q.y() - q.x() * q.z()), 1 - 2 * (q.x() * q.x() + q.z() * q.z()));
        yaw = std::asin(2 * (q.scalar() * q.x() - q.y() * q.z()));
    }
    if (order == "Y-X-Y") {
        roll = std::atan2(2 * (q.scalar() * q.y() + q.x() * q.z()), 1 - 2 * (q.y() * q.y() + q.z() * q.z()));
        pitch = std::atan2(2 * (q.scalar() * q.x() - q.z() * q.y()), 1 - 2 * (q.x() * q.x() + q.y() * q.y()));
        yaw = std::asin(2 * (q.scalar() * q.y() - q.x() * q.z()));
    }
    if (order == "Y-Z-Y") {
        roll = std::atan2(2 * (q.scalar() * q.y() + q.z() * q.x()), 1 - 2 * (q.y() * q.y() + q.x() * q.x()));
        pitch = std::atan2(2 * (q.scalar() * q.z() - q.x() * q.y()), 1 - 2 * (q.x() * q.x() + q.z() * q.z()));
        yaw = std::asin(2 * (q.scalar() * q.y() - q.x() * q.z()));
    }
    if (order == "Z-X-Z") {
        roll = std::atan2(2 * (q.scalar() * q.z() + q.x() * q.y()), 1 - 2 * (q.z() * q.z() + q.y() * q.y()));
        pitch = std::atan2(2 * (q.scalar() * q.x() - q.z() * q.y()), 1 - 2 * (q.x() * q.x() + q.z() * q.z()));
        yaw = std::asin(2 * (q.scalar() * q.z() - q.x() * q.y()));
    }
    if (order == "Z-Y-Z") {
        roll = std::atan2(2 * (q.scalar() * q.z() + q.y() * q.x()), 1 - 2 * (q.z() * q.z() + q.x() * q.x()));
        pitch = std::atan2(2 * (q.scalar() * q.y() - q.z() * q.x()), 1 - 2 * (q.y() * q.y() + q.z() * q.z()));
        yaw = std::asin(2 * (q.scalar() * q.z() - q.x() * q.y()));
    }
    // else {
    //     qDebug() << "Unknown order!";
    //     return;
    // }

    return QVector3D(qRadiansToDegrees(roll), qRadiansToDegrees(pitch), qRadiansToDegrees(yaw));
}

Axis* Axis::airAngles(float ang0, float ang1, float ang2, const QString& order) { // работает
    ang0 = reduceAng(ang0);
    ang1 = reduceAng(ang1);
    ang2 = reduceAng(ang2);

    QQuaternion q0;
    QQuaternion q1;
    QQuaternion q2;


    if (order == "X-Y-Z") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang2);
    } else if (order == "X-Z-Y") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang2);
    } else if (order == "Y-Z-X") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang2);
    } else if (order == "Y-X-Z") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang2);
    } else if (order == "Z-X-Y") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang2);
    } else if (order == "Z-Y-X") {
        q0 = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), ang0);
        q1 = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), ang1);
        q2 = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), ang2);
    }

    QQuaternion finalQuaternion = q2*q1*q0;
    finalQuaternion.normalize();
    currentQuaernion = finalQuaternion;
    quater = whatQuater(ang1);

    QQuaternion qByHands;
    if (order == "X-Y-Z") {
        qByHands = quatByHands(ang0, ang1, ang2);
        qByHands.normalize();
        qDebug() << "FFFFFFF";
    }
    qDebug() << " fqw = " << finalQuaternion.scalar() << "\n fqx = " << finalQuaternion.x() << "\n fqy = " << finalQuaternion.y()
             << "\n fqz = " << finalQuaternion.z();

    qDebug() << "";

    qDebug() << " qw = " << qByHands.scalar() << "\n qx = " << qByHands.x() << "\n qy = " << qByHands.y()
             << "\n qz = " << qByHands.z();

    // конец тестовой части

    Line *newXAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(xAxis->end()), QColor("red"));
    Line *newYAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(yAxis->end()), QColor("green"));
    Line *newZAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(zAxis->end()), QColor("blue"));

    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;

    return newAxis;
}

Axis* Axis::aircraftAngles(float ang0, float ang1, float ang2, const QString& order) { // устаревший вариант, но рабочий
    // Уопрядочиваем углы в правльную последовательность для работы
    QVector3D vec = createVector(ang0, ang1, ang2, order);

    // Кватернионы для осей
    QQuaternion qRoll = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), vec.x());
    QQuaternion qPitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), vec.y());
    QQuaternion qYaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), vec.z());

    QQuaternion finalQuaternion;

    // Порядок введения
    if (order == "X-Y-Z") {
        finalQuaternion = qYaw * qPitch * qRoll;

    } else if (order == "X-Z-Y") {
        finalQuaternion = qPitch * qYaw * qRoll;
    } else if (order == "Y-Z-X") {
        finalQuaternion = qRoll * qYaw * qPitch;
    } else if (order == "Y-X-Z") {
        finalQuaternion = qYaw * qRoll * qPitch;
    } else if (order == "Z-X-Y") {
        finalQuaternion = qPitch * qRoll * qYaw;
    } else if (order == "Z-Y-X") {
        finalQuaternion = qRoll * qPitch * qYaw;
    } else if (order == "Z-Y-X") {
        finalQuaternion = qRoll * qPitch * qYaw;
    }

    finalQuaternion.normalize();
    currentQuaernion = finalQuaternion;

    Line *newXAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(xAxis->end()), QColor("red"));
    Line *newYAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(yAxis->end()), QColor("green"));
    Line *newZAxis = new Line(QVector3D(0, 0, 0), finalQuaternion.rotatedVector(zAxis->end()), QColor("blue"));

    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;

    return newAxis;
}

// Функция для поворота вектора вокруг оси X
QVector3D Axis::rotateAroundX(const QVector3D& v, float angle) {
    float rad = qDegreesToRadians(angle);
    float cosA = qCos(rad);
    float sinA = qSin(rad);

    // Поворачиваем вектор вокруг оси X
    return QVector3D(v.x(), v.y() * cosA - v.z() * sinA, v.y() * sinA + v.z() * cosA);
}

// Функция для поворота вектора вокруг оси Y
QVector3D Axis::rotateAroundY(const QVector3D& v, float angle) {
    float rad = qDegreesToRadians(angle);
    float cosA = qCos(rad);
    float sinA = qSin(rad);

    // Поворачиваем вектор вокруг оси Y
    return QVector3D(v.x() * cosA + v.z() * sinA, v.y(), -v.x() * sinA + v.z() * cosA);
}

// Функция для поворота вектора вокруг оси Z
QVector3D Axis::rotateAroundZ(const QVector3D& v, float angle) {
    float rad = qDegreesToRadians(angle);
    float cosA = qCos(rad);
    float sinA = qSin(rad);

    // Поворачиваем вектор вокруг оси Z
    return QVector3D(v.x() * cosA - v.y() * sinA, v.x() * sinA + v.y() * cosA, v.z());
}

// Функция для применения самолётных углов (Roll, Pitch, Yaw) вокруг глобальных осей
Axis* Axis::applyAircraftAnglesGlobal(float roll, float pitch, float yaw, const QString& order) {
    QVector3D newX = xAxis->end();
    QVector3D newY = yAxis->end();
    QVector3D newZ = zAxis->end();

    // Применяем вращения в зависимости от переданного порядка осей
    if (order == "X-Y-Z") {
        // Поворот сначала вокруг оси X, затем Y, затем Z
        newX = rotateAroundX(newX, roll);
        newX = rotateAroundY(newX, pitch);
        newX = rotateAroundZ(newX, yaw);

        newY = rotateAroundX(newY, roll);
        newY = rotateAroundY(newY, pitch);
        newY = rotateAroundZ(newY, yaw);

        newZ = rotateAroundX(newZ, roll);
        newZ = rotateAroundY(newZ, pitch);
        newZ = rotateAroundZ(newZ, yaw);
    } else if (order == "X-Z-Y") {
        newX = rotateAroundX(newX, roll);
        newX = rotateAroundZ(newX, yaw);
        newX = rotateAroundY(newX, pitch);

        newY = rotateAroundX(newY, roll);
        newY = rotateAroundZ(newY, yaw);
        newY = rotateAroundY(newY, pitch);

        newZ = rotateAroundX(newZ, roll);
        newZ = rotateAroundZ(newZ, yaw);
        newZ = rotateAroundY(newZ, pitch);
    } else if (order == "Y-X-Z") {
        newX = rotateAroundY(newX, pitch);
        newX = rotateAroundX(newX, roll);
        newX = rotateAroundZ(newX, yaw);

        newY = rotateAroundY(newY, pitch);
        newY = rotateAroundX(newY, roll);
        newY = rotateAroundZ(newY, yaw);

        newZ = rotateAroundY(newZ, pitch);
        newZ = rotateAroundX(newZ, roll);
        newZ = rotateAroundZ(newZ, yaw);
    } else if (order == "Y-Z-X") {
        newX = rotateAroundY(newX, pitch);
        newX = rotateAroundZ(newX, yaw);
        newX = rotateAroundX(newX, roll);

        newY = rotateAroundY(newY, pitch);
        newY = rotateAroundZ(newY, yaw);
        newY = rotateAroundX(newY, roll);

        newZ = rotateAroundY(newZ, pitch);
        newZ = rotateAroundZ(newZ, yaw);
        newZ = rotateAroundX(newZ, roll);
    } else if (order == "Z-X-Y") {
        newX = rotateAroundZ(newX, yaw);
        newX = rotateAroundX(newX, roll);
        newX = rotateAroundY(newX, pitch);

        newY = rotateAroundZ(newY, yaw);
        newY = rotateAroundX(newY, roll);
        newY = rotateAroundY(newY, pitch);

        newZ = rotateAroundZ(newZ, yaw);
        newZ = rotateAroundX(newZ, roll);
        newZ = rotateAroundY(newZ, pitch);
    } else if (order == "Z-Y-X") {
        newX = rotateAroundZ(newX, yaw);
        newX = rotateAroundY(newX, pitch);
        newX = rotateAroundX(newX, roll);

        newY = rotateAroundZ(newY, yaw);
        newY = rotateAroundY(newY, pitch);
        newY = rotateAroundX(newY, roll);

        newZ = rotateAroundZ(newZ, yaw);
        newZ = rotateAroundY(newZ, pitch);
        newZ = rotateAroundX(newZ, roll);
    } else {
        // Если порядок не распознан, выводим предупреждение и возвращаем оригинальные оси без изменений
        qWarning() << "Unknown rotation order: " << order;
        return this;
    }

    // Применяем результат вращения к осям
    Line* newXAxis = new Line(QVector3D(0, 0, 0), newX, QColor("orange"));
    Line* newYAxis = new Line(QVector3D(0, 0, 0), newY, QColor("brown"));
    Line* newZAxis = new Line(QVector3D(0, 0, 0), newZ, QColor("purple"));

    // Создаём и возвращаем новый Axis с обновлёнными осями
    Axis* newAxis = new Axis();
    newAxis->xAxis = newXAxis;
    newAxis->yAxis = newYAxis;
    newAxis->zAxis = newZAxis;

    return newAxis;
}
