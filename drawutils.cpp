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