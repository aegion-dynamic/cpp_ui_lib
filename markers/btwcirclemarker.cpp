#include "btwcirclemarker.h"
#include "../waterfallgraph.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QFont>
#include <QDebug>
#include <QtMath>

// BTWCircleMarker implementation
BTWCircleMarker::BTWCircleMarker(const QDateTime &timestamp, qreal range, qreal delta)
    : NonInteractiveWaterfallMarker(MarkerType::BTWCircleMarker, timestamp, range)
    , m_delta(delta)
{
}

void BTWCircleMarker::draw(QGraphicsScene *scene, WaterfallGraph *graph)
{
    if (!scene || !graph) {
        return;
    }

    // Map data coordinates to screen coordinates
    QPointF screenPos = graph->mapDataToScreenPublic(m_value, m_timestamp);
    
    // Check if point is within visible area
    QRectF drawingArea = graph->getDrawingArea();
    if (!drawingArea.contains(screenPos)) {
        return;
    }

    // Calculate marker size based on window size
    QSize windowSize = graph->size();
    qreal markerRadius = std::min(0.04 * windowSize.width(), 12.0); // Circle radius, cap at 12 pixels
    
    // Draw circle outline
    QGraphicsEllipseItem *circleOutline = new QGraphicsEllipseItem();
    circleOutline->setRect(screenPos.x() - markerRadius, screenPos.y() - markerRadius, 
                         2 * markerRadius, 2 * markerRadius);
    circleOutline->setPen(QPen(Qt::blue, 2));
    circleOutline->setBrush(QBrush(Qt::transparent));
    circleOutline->setZValue(1000);
    
    scene->addItem(circleOutline);
    
    // Draw angled line (5x radius on both sides)
    qreal lineLength = 5 * markerRadius;
    
    // Calculate angle from delta value
    // Map delta value to angle: positive delta = positive angle (clockwise), negative delta = negative angle (counterclockwise)
    qreal angleDegrees = m_delta * 10.0; // Scale factor to convert delta to meaningful angle
    qreal angleRadians = qDegreesToRadians(angleDegrees);
    
    // Calculate line endpoints based on angle
    // For true north (0°), line points up/down (vertical)
    qreal deltaX = lineLength * qSin(angleRadians);
    qreal deltaY = -lineLength * qCos(angleRadians); // Negative because Y increases downward
    
    QGraphicsLineItem *angledLine = new QGraphicsLineItem();
    angledLine->setLine(screenPos.x() - deltaX, screenPos.y() - deltaY,
                      screenPos.x() + deltaX, screenPos.y() + deltaY);
    angledLine->setPen(QPen(Qt::blue, 2));
    angledLine->setZValue(1001);
    
    scene->addItem(angledLine);
    
    // Add blue text label with rectangular outline beside the marker
    QString prefix = (m_delta >= 0) ? "R" : "L";
    QString displayValue = (m_delta >= 0) ? QString::number(m_delta, 'f', 1) : QString::number(-m_delta, 'f', 1);
    QGraphicsTextItem *textLabel = new QGraphicsTextItem(prefix + displayValue);
    QFont font = textLabel->font();
    font.setPointSizeF(8.0);
    font.setBold(true);
    textLabel->setFont(font);
    textLabel->setDefaultTextColor(Qt::blue);
    
    // Position text label to the left of the marker
    QRectF textRect = textLabel->boundingRect();
    textLabel->setPos(screenPos.x() - textRect.width() - markerRadius - 5, 
                    screenPos.y() - textRect.height() / 2);
    textLabel->setZValue(1002);
    
    scene->addItem(textLabel);
    
    // Add rectangular outline around the text
    QGraphicsRectItem *textOutline = new QGraphicsRectItem();
    textOutline->setRect(textLabel->pos().x() - 2, textLabel->pos().y() - 2,
                       textRect.width() + 4, textRect.height() + 4);
    textOutline->setPen(QPen(Qt::blue, 1));
    textOutline->setBrush(QBrush(Qt::transparent));
    textOutline->setZValue(1001);
    
    scene->addItem(textOutline);
}

