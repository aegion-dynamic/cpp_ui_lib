#include "drawutils.h"

/**
 * @brief converts to bearing to radians
 * 
 * @param bearing 
 * @return double 
 */
double DrawUtils::bearingtoRadians(double bearing) {

    auto degree = (90.0f - bearing);

    return qDegreesToRadians(degree);
}

/**
 * @brief calculates the endpoint from a start point and the polar arguments
 * 
 * @param startPoint 
 * @param magnitude 
 * @param bearing 
 * @return QPointF 
 */
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

/**
 * @brief Converts bearing to cartesian points
 * 
 * @param magnitude 
 * @param bearing 
 * @param window 
 * @return QPointF 
 */
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

/**
 * @brief Added the test pattern
 * 
 * @param scene 
 * @param rectToDraw 
 */
void DrawUtils::addTestPattern(QGraphicsScene* scene, QRectF rectToDraw)
{
    if (!scene)
    {
        qDebug() << "No scene found, existing DrawUtils::addTestPattern";
        return;
    }

    QPen testPen(Qt::white, 1);
    testPen.setStyle(Qt::DashLine);

    // Draw test rectangle showing full bounds
    scene->addRect(rectToDraw, testPen);

}


/**
 * @brief Draw a test pattern for debugging purposes.
 *
 */
void DrawUtils::drawDefaultTestPattern(QGraphicsScene* scene)
{
    if (!scene)
        return;

    QPen testPen(Qt::white, 1);
    testPen.setStyle(Qt::DashLine);

    // Draw test rectangle showing full bounds
    scene->addRect(scene->sceneRect(), testPen);

    // Draw diagonal line to show extent
    scene->addLine(0, 0, scene->sceneRect().width(), scene->sceneRect().height(), testPen);
}




/**
 * Computes a transformation matrix to transform sourceRect to fit within targetRect.
 * The transformation will:
 * 1. Scale sourceRect so its largest dimension equals targetRect's smallest dimension
 * 2. Center the scaled sourceRect within targetRect
 * 
 * @param sourceRect The rectangle to be transformed
 * @param targetRect The target rectangle to fit within
 * @return QTransform matrix that applies the required transformation
 */
QTransform DrawUtils::computeTransformationMatrix(const QRectF& sourceRect, const QRectF& targetRect)
{
    // Handle degenerate cases
    if (sourceRect.isEmpty() || targetRect.isEmpty()) {
        return QTransform(); // Return identity transform
    }
    
    // Get dimensions of both rectangles
    qreal sourceWidth = sourceRect.width();
    qreal sourceHeight = sourceRect.height();
    qreal targetWidth = targetRect.width();
    qreal targetHeight = targetRect.height();
    
    // Find the largest dimension of source rectangle
    qreal sourceLargestDimension = std::max(sourceWidth, sourceHeight);
    
    // Find the smallest dimension of target rectangle
    qreal targetSmallestDimension = std::min(targetWidth, targetHeight);
    
    // Calculate scale factor
    qreal scaleFactor = targetSmallestDimension / sourceLargestDimension;
    
    // Calculate the centers of both rectangles
    QPointF sourceCenter = sourceRect.center();
    QPointF targetCenter = targetRect.center();
    
    // Create the transformation matrix
    QTransform transform;
    
    // Step 1: Translate source rectangle so its center is at origin
    transform.translate(-sourceCenter.x(), -sourceCenter.y());
    
    // Step 2: Apply uniform scaling
    transform.scale(scaleFactor, scaleFactor);
    
    // Step 3: Translate to target center
    transform.translate(targetCenter.x(), targetCenter.y());
    
    return transform;
}

/**
 * Alternative version that returns the transformed rectangle for verification
 */
QPair<QTransform, QRectF> DrawUtils::computeTransformationWithResult(const QRectF& sourceRect, const QRectF& targetRect)
{
    QTransform transform = computeTransformationMatrix(sourceRect, targetRect);
    QRectF transformedRect = transform.mapRect(sourceRect);
    
    return qMakePair(transform, transformedRect);
}

// /**
//  * Example usage function
//  */
// void exampleUsage()
// {
//     // Example rectangles
//     QRectF sourceRect(10, 20, 100, 50);  // Rectangle at (10,20) with size 100x50
//     QRectF targetRect(0, 0, 200, 300);   // Rectangle at (0,0) with size 200x300
    
//     // Compute transformation
//     QTransform transform = computeTransformationMatrix(sourceRect, targetRect);
    
//     // Apply transformation to source rectangle
//     QRectF transformedRect = transform.mapRect(sourceRect);
    
//     // The transformed rectangle should now be centered in targetRect
//     // with its largest dimension (100) scaled to match targetRect's smallest dimension (200)
//     // So the scale factor would be 200/100 = 2.0
//     // Final size would be 200x100, centered at (100, 150)
// }