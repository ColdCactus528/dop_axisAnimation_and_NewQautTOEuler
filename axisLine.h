#ifndef AXISLINE_H
#define AXISLINE_H

#include <QQuick3DGeometry>
#include <QVector3D>
#include <QColor>

class Line : public QQuick3DGeometry {
    Q_OBJECT
    QML_NAMED_ELEMENT(Line)

    // Определение свойств с использованием макроса Q_PROPERTY
    Q_PROPERTY(QVector3D start READ start WRITE setStart NOTIFY startChanged)
    Q_PROPERTY(QVector3D end READ end WRITE setEnd NOTIFY endChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    // Конструкторы класса
    Line();
    Line(const QVector3D &start, const QVector3D &end, const QColor &color = QColor("red"));

    // Методы для работы со свойствами start, end и color
    QVector3D start() const { return m_start; }
    void setStart(const QVector3D &start);

    QVector3D end() const { return m_end; }
    void setEnd(const QVector3D &end);

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

signals:
    // Сигналы для отслеживания изменений свойств
    void startChanged();
    void endChanged();
    void colorChanged();

private:
    // Метод для обновления данных геометрии линии
    void updateData();

    // Приватные переменные для хранения начальных и конечных координат и цвета линии
    QVector3D m_start;
    QVector3D m_end;
    QColor m_color;
};

#endif // AXISLINE_H
