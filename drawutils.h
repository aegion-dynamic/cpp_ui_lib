#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include <QGraphicsItem>

class DrawUtils{

    public:
        // QGraphicsItem* drawCourse(QPointF startPoint, QPointF endPoint);
        // QGraphicsPolygonItem* drawPolygon(const std::vector<QPointF>& points);
        // QGraphicsLineItem* drawBearingLine(QPointF origin, double magnitude);
        static double bearingtoRadians(double bearing);
        static QPointF calculateEndpoint(QPointF startPoint, double magnitude, double bearing);
};

#endif //DRAWUTILS_H
