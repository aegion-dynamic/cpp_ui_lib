#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTransform>
#include <QRectF>
#include <QPointF>
#include <algorithm>

class DrawUtils
{

public:
    // QGraphicsItem* drawCourse(QPointF startPoint, QPointF endPoint);
    // QGraphicsPolygonItem* drawPolygon(const std::vector<QPointF>& points);
    // QGraphicsLineItem* drawBearingLine(QPointF origin, double magnitude);
    static double bearingtoRadians(double bearing);
    static QPointF calculateEndpoint(QPointF startPoint, double magnitude, double bearing);
    static void drawCourseVector(QGraphicsScene *scene, QPointF startPoint, double magnitude, double bearing, const QColor &color);
    static QPointF bearingToCartesian(qreal magnitude, qreal bearing, QRectF window);
    static void addTestPattern(QGraphicsScene *scene, QRectF rectToDraw);
    static void drawDefaultTestPattern(QGraphicsScene *scene);
    static QTransform computeTransformationMatrix(const QRectF &sourceRect, const QRectF &targetRect);
    static QPair<QTransform, QRectF> computeTransformationWithResult(const QRectF &sourceRect, const QRectF &targetRect);
    static void transformAllSceneItems(QGraphicsScene *scene, const QTransform &transform);
    static QGraphicsLineItem *createLineFromPointAndAngle(const QPointF &startPoint,
                                                          qreal angleInDegrees,
                                                          qreal length);
    static qreal computeCartesianDistance(QPointF source, QPointF target);
    static void addTestLine(QGraphicsScene* scene, QLineF lineToDraw);
    static qreal flipBearing(qreal bearing);


};

#endif // DRAWUTILS_H
