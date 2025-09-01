#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include <QGraphicsItem>
#include <QGraphicsScene>

class DrawUtils{

    public:
        // QGraphicsItem* drawCourse(QPointF startPoint, QPointF endPoint);
        // QGraphicsPolygonItem* drawPolygon(const std::vector<QPointF>& points);
        // QGraphicsLineItem* drawBearingLine(QPointF origin, double magnitude);
        static double bearingtoRadians(double bearing);
        static QPointF calculateEndpoint(QPointF startPoint, double magnitude, double bearing);
        static void drawCourseVector(QGraphicsScene* scene, QPointF startPoint, double magnitude, double bearing, const QColor& color);
        static QPointF bearingToCartesian(qreal magnitude, qreal bearing, QRectF window);

};

#endif //DRAWUTILS_H
