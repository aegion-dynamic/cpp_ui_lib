#include "drawutils.h"

double DrawUtils::bearingtoRadians(double bearing) {

    auto degree = (90.0f - bearing);

    return qDegreesToRadians(degree);
}

QPointF DrawUtils::calculateEndpoint(QPointF startPoint, double magnitude, double bearing)
{
    // Convert bearing to radians
    double bearingRad = DrawUtils::bearingtoRadians(bearing);

    // Calculate endpoint
    double endX = startPoint.x() + magnitude * qCos(bearingRad);
    double endY = startPoint.y() - magnitude * qSin(bearingRad);

    return QPointF(endX, endY);
}

/// @brief Draw individual course vectors
/// @param scene 
/// @param startPoint 
/// @param magnitude 
/// @param bearing 
/// @param color 
void DrawUtils::drawCourseVector(QGraphicsScene *scene, QPointF startPoint, double magnitude, double bearing, const QColor &color)
{

    qreal headLen = 5;
    qreal headAngleDeg = 30;

    QPen pen(color);
    QBrush brush(color);
    int radius = 5;
    scene->addEllipse(startPoint.x() - radius, startPoint.y() - radius, radius * 2, radius * 2, pen, brush);

    // Calulate endpoint

    auto endpoint = DrawUtils::calculateEndpoint(startPoint, magnitude, bearing);

    pen.setWidth(2);
    scene->addLine(QLineF(startPoint, endpoint), pen);

    // Calculate arrow head points
    qreal angle = qAtan2(endpoint.y() - startPoint.y(), endpoint.x() - startPoint.x());
    qreal a1 = angle + qDegreesToRadians(180.0 - headAngleDeg);
    qreal a2 = angle - qDegreesToRadians(180.0 - headAngleDeg);

    QPointF h1(endpoint.x() + headLen * qCos(a1), endpoint.y() + headLen * qSin(a1));
    QPointF h2(endpoint.x() + headLen * qCos(a2), endpoint.y() + headLen * qSin(a2));
    
    // Draw arrow head as filled polygon
    QPolygonF head; 
    
    head << endpoint << h1 << h2;
    
    scene->addPolygon(head, pen, brush);
    
}

QPointF DrawUtils::bearingToCartesian(qreal magnitude, qreal bearing, QRectF window)
{
    // Center coorindate
    QPointF center = QPointF(
        window.x() + window.width()/2,
        window.y() + window.height()/2
    );

    // Convert the bearing to a normal degree angle
    auto degree = 90 - bearing;

    // Convert to radians
    auto radians = qDegreesToRadians(degree);

    // Calculate the endpoint
    qreal x = center.x() + magnitude * qCos(radians);
    qreal y = center.y() - magnitude * qSin(radians);

    return QPointF(x, y);
}
