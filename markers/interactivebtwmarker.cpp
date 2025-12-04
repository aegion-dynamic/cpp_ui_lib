#include "interactivebtwmarker.h"
#include "../waterfallgraph.h"
#include "../interactivegraphicsitem.h"
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QFont>
#include <QFontMetrics>
#include <QVariant>
#include <QDebug>
#include <QtMath>

// InteractiveBTWMarker implementation
InteractiveBTWMarker::InteractiveBTWMarker(const QDateTime &timestamp, qreal value, const QPointF &position, QGraphicsScene *overlayScene)
    : InteractiveWaterfallMarker(MarkerType::InteractiveBTWMarker, timestamp, value)
    , m_item(nullptr)
    , m_overlayScene(overlayScene)
{
    setupInteractiveItem();
    if (m_item) {
        m_item->setPos(position);
        // Store timestamp in marker data for later retrieval
        m_item->setData(0, QVariant::fromValue(timestamp));
        // Store value (range) in marker data
        m_item->setData(1, QVariant::fromValue(value));
    }
}

InteractiveBTWMarker::~InteractiveBTWMarker()
{
    removeBearingRateBox();
    if (m_item && m_overlayScene) {
        m_overlayScene->removeItem(m_item);
    }
    if (m_item) {
        delete m_item;
        m_item = nullptr;
    }
}

void InteractiveBTWMarker::setupInteractiveItem()
{
    if (!m_overlayScene) {
        qDebug() << "InteractiveBTWMarker: Cannot create item - no overlay scene";
        return;
    }

    m_item = new InteractiveGraphicsItem();
    m_item->setSize(QSizeF(20, 20));

    // Set custom drawing function for data point - green version without interaction regions
    m_item->setCustomDrawFunction([this](QPainter *painter, const QRectF &rect) {
        Q_UNUSED(rect);
        
        // Calculate marker radius based on the original size (20x20)
        qreal markerRadius = 10.0; // Half of the 20x20 size
        
        // Draw green circle outline (transparent fill) at the center of the item
        painter->setPen(QPen(Qt::green, 2));
        painter->setBrush(QBrush(Qt::transparent));
        QRectF circleRect(-markerRadius, -markerRadius, 2*markerRadius, 2*markerRadius);
        painter->drawEllipse(circleRect);
        
        // Draw angled line through center
        // Use the rotation angle of the marker
        qreal angleDegrees = m_item->rotation();
        qreal angleRadians = qDegreesToRadians(angleDegrees);
        qreal lineLength = 5 * markerRadius; // Same as BTW custom markers
        
        // Calculate line endpoints based on angle
        // For true north (0°), line points up/down (vertical)
        qreal deltaX = lineLength * qSin(angleRadians);
        qreal deltaY = -lineLength * qCos(angleRadians); // Negative because Y increases downward
        
        // Use the center of the item (0,0) as the reference point
        QPointF startPoint = QPointF(-deltaX, -deltaY);
        QPointF endPoint = QPointF(deltaX, deltaY);
        
        // Draw the angled line in green
        painter->setPen(QPen(Qt::green, 2));
        painter->drawLine(startPoint, endPoint);
    });

    // Apply styling and configure interaction regions
    QPen dataPointPen(Qt::green, 2);
    QBrush dataPointBrush(Qt::transparent);
    m_item->setDragRegionPen(dataPointPen);
    m_item->setDragRegionBrush(dataPointBrush);
    m_item->setShowDragRegion(false);  // Hide the drag region square
    m_item->setShowRotateRegion(false); // Hide the rotate regions at line ends
    m_item->setRotateRegionSize(QSizeF(12, 12)); // Set rotation regions to 12x12 pixels

    // Connect signals for movement and rotation
    // When marker is moved, update timestamp/value based on new position
    // When marker is rotated, just update bearing rate box
    QObject::connect(m_item, &InteractiveGraphicsItem::itemMoved, [this](const QPointF &newPosition) {
        // Update timestamp and value based on new position
        // Note: We need access to the graph for mapping, but we'll handle this in draw()
        // For now, just update bearing rate box
        updateBearingRateBox();
    });
    
    QObject::connect(m_item, &InteractiveGraphicsItem::itemRotated, [this](qreal angle) {
        Q_UNUSED(angle);
        updateBearingRateBox();
    });

    // Add to scene
    m_overlayScene->addItem(m_item);
    
    // Create initial bearing rate box
    updateBearingRateBox();
}

void InteractiveBTWMarker::draw(QGraphicsScene *scene, WaterfallGraph *graph)
{
    if (!m_item || !graph) {
        return;
    }
    
    // Update marker position based on current time/value axis mapping
    // This ensures markers stay correctly positioned when graph view changes
    QPointF newPosition = graph->mapDataToScreenPublic(m_value, m_timestamp);
    
    // Check if marker was moved by user (position differs from calculated position)
    QPointF currentPos = m_item->pos();
    qreal positionTolerance = 5.0; // Allow small tolerance for floating point differences
    
    bool userMovedMarker = (QPointF(currentPos - newPosition).manhattanLength() > positionTolerance);
    
    if (userMovedMarker) {
        // User moved the marker - update timestamp/value based on new position
        m_timestamp = graph->mapScreenToTimePublic(currentPos.y());
        m_value = graph->mapScreenXToRange(currentPos.x());
        
        // Recalculate position with new values to ensure accuracy
        newPosition = graph->mapDataToScreenPublic(m_value, m_timestamp);
        m_item->setPos(newPosition);
        
        qDebug() << "InteractiveBTWMarker: Updated timestamp/value based on user movement - timestamp:" 
                 << m_timestamp.toString() << "value:" << m_value;
    } else {
        // Graph view changed - update position to match new mapping
        if (currentPos != newPosition) {
            m_item->setPos(newPosition);
        }
    }
    
    // Ensure item is in the correct scene
    if (scene && m_item->scene() != scene) {
        if (m_item->scene()) {
            m_item->scene()->removeItem(m_item);
        }
        scene->addItem(m_item);
    }
    
    // Update bearing rate box with new position
    updateBearingRateBox();
}

void InteractiveBTWMarker::updatePosition(WaterfallGraph *graph)
{
    if (!m_item || !graph) {
        return;
    }
    
    // Update position based on current time/value axis mapping
    QPointF newPosition = graph->mapDataToScreenPublic(m_value, m_timestamp);
    m_item->setPos(newPosition);
    
    // Update bearing rate box after position change
    updateBearingRateBox();
}

void InteractiveBTWMarker::updateBearingRateBox()
{
    if (!m_item || !m_overlayScene) {
        return;
    }
    
    // Remove old bearing rate items if they exist
    removeBearingRateBox();
    
    // Get marker position (use scene position for absolute coordinates)
    QPointF markerPos = m_item->scenePos();
    qreal markerRadius = 10.0; // Match the marker radius
    qreal bearingRate = m_item->rotation(); // Use the rotation angle as the bearing rate
    
    // Format the bearing rate text with R/L prefix (no decimal places)
    QString prefix = (bearingRate == 0) ? "" : (bearingRate >= 0) ? "R" : "L";
    QString displayValue = (bearingRate >= 0) ? QString::number(bearingRate, 'f', 0) : QString::number(-bearingRate, 'f', 0);
    QString bearingRateText = prefix + displayValue;
    
    // Set up font
    QFont font;
    font.setPointSizeF(8.0);
    font.setBold(true);
    
    // Calculate text dimensions
    QFontMetrics fm(font);
    QRectF textRect = fm.boundingRect(bearingRateText);
    
    // Position text to the left of the marker (centered vertically)
    qreal textX = markerPos.x() - textRect.width() - markerRadius - 7;
    qreal textY = markerPos.y() - textRect.height() / 2;
    
    // Create and add text label
    QGraphicsTextItem *textLabel = new QGraphicsTextItem(bearingRateText);
    textLabel->setFont(font);
    textLabel->setDefaultTextColor(Qt::green);
    textLabel->setPos(textX, textY);
    textLabel->setZValue(1002);
    
    // Create and add rectangular outline around the text
    QGraphicsRectItem *textOutline = new QGraphicsRectItem();
    textOutline->setRect(textX - 2, textY + 1, textRect.width() + 6, textRect.height() + 4);
    textOutline->setPen(QPen(Qt::green, 1));
    textOutline->setBrush(QBrush(Qt::transparent));
    textOutline->setZValue(1001);
    
    m_overlayScene->addItem(textLabel);
    m_overlayScene->addItem(textOutline);
    
    m_bearingRateItems.append(textLabel);
    m_bearingRateItems.append(textOutline);
}

void InteractiveBTWMarker::removeBearingRateBox()
{
    if (!m_overlayScene) {
        return;
    }
    
    // Remove stored bearing rate items from scene
    for (QGraphicsItem *item : m_bearingRateItems) {
        if (item) {
            m_overlayScene->removeItem(item);
            delete item;
        }
    }
    m_bearingRateItems.clear();
}

