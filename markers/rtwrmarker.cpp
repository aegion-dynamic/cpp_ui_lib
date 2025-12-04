#include "rtwrmarker.h"
#include "../waterfallgraph.h"
#include <QGraphicsTextItem>
#include <QFont>
#include <QDebug>

// RTWRMarker implementation
RTWRMarker::RTWRMarker(const QDateTime &timestamp, qreal range)
    : NonInteractiveWaterfallMarker(MarkerType::RTWRMarker, timestamp, range)
{
}

void RTWRMarker::draw(QGraphicsScene *scene, WaterfallGraph *graph)
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
    qreal markerSize = std::min(0.08 * windowSize.width(), 24.0); // Increased size, cap at 24 pixels
    
    // Create yellow "R" text marker
    QGraphicsTextItem *rMarker = new QGraphicsTextItem("R");
    QFont font = rMarker->font();
    font.setPointSizeF(markerSize);
    font.setBold(true);
    rMarker->setFont(font);
    rMarker->setDefaultTextColor(Qt::yellow);
    
    // Center the marker on the data point
    QRectF textRect = rMarker->boundingRect();
    rMarker->setPos(screenPos.x() - textRect.width()/2, screenPos.y() - textRect.height()/2);
    rMarker->setZValue(1000); // Very high z-value to ensure visibility
    
    // Make marker explicitly accept mouse events for reliable clicking
    rMarker->setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    rMarker->setAcceptHoverEvents(true);
    
    scene->addItem(rMarker);
}

