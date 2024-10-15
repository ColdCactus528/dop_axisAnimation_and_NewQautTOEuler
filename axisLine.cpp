#include "axisLine.h"
#include <QVector3D>

Line::Line()
    : m_start(QVector3D(0.0f, 0.0f, 0.0f)), m_end(QVector3D(0.0f, 50.0f, 0.0f)), m_color(Qt::red) { // Инициализируем точки в конструкторе
    updateData();  // Обновляем данные для отрисовки
}

Line::Line(const QVector3D &start, const QVector3D &end, const QColor &color)
    : m_start(start), m_end(end), m_color(color) {
    updateData();
}

void Line::setStart(const QVector3D &start){
    if(m_start != start) {
        m_start = start;
        emit startChanged();
        updateData();
    }
}

void Line::setEnd(const QVector3D &end){
    if(m_end != end) {
        m_end = end;
        emit endChanged();
        updateData();
    }
}

void Line::setColor(const QColor &color){
    if(m_color != color) {
        m_color = color;
        emit colorChanged();
        updateData();
    }
}

void Line::updateData() {
    clear();  // Очищаем предыдущие данные

    // Размер шага (stride) - три компоненты (X, Y, Z)
    const int stride = 3 * sizeof(float);
    QByteArray vertexData;
    vertexData.resize(2 * stride);  // Два конца линии (2 точки)
    float *p = reinterpret_cast<float *>(vertexData.data());

    // Начальная точка линии
    *p++ = m_start.x();
    *p++ = m_start.y();
    *p++ = m_start.z();

    // Конечная точка линии
    *p++ = m_end.x();
    *p++ = m_end.y();
    *p++ = m_end.z();

    // qDebug() << "Updating Line from" << m_start << "to" << m_end;

    // Передаем данные геометрии
    setVertexData(vertexData);
    setStride(stride);
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);  // Задаем тип примитива: линия

    // Добавляем атрибут позиции для геометрии (3D координаты)
    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic, 0, QQuick3DGeometry::Attribute::F32Type);
}
