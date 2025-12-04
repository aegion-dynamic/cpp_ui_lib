#include "btwgraph.h"
#include "interactivegraphicsitem.h"
#include "graphcontainer.h"
#include "graphlayout.h"
#include "waterfalldata.h"
#include "markers/waterfallmarker.h"
#include "markers/btwcirclemarker.h"
#include "markers/interactivebtwmarker.h"
#include "graphtype.h"
#include <QDebug>
#include <QRandomGenerator>
#include <memory>

/**
 * @brief Construct a new BTWGraph::BTWGraph object
 *
 * @param parent Parent widget
 * @param enableGrid Whether to enable grid display
 * @param gridDivisions Number of grid divisions
 * @param timeInterval Time interval for the waterfall display
 */
BTWGraph::BTWGraph(QWidget *parent, bool enableGrid, int gridDivisions, TimeInterval timeInterval)
    : WaterfallGraph(parent, enableGrid, gridDivisions, timeInterval)
    , symbols(40)  // Initialize BTW symbol cache
{
    qDebug() << "BTWGraph constructor called";
    
    // Interactive markers are created directly via InteractiveBTWMarker
    // WaterfallGraph handles drawing and position updates
}

/**
 * @brief Destroy the BTWGraph::BTWGraph object
 *
 */
BTWGraph::~BTWGraph()
{
    qDebug() << "BTWGraph destructor called";
}

/**
 * @brief Override draw method to create scatterplots by default
 *
 */
void BTWGraph::draw()
{
    if (!graphicsScene)
        return;
    
    // Prevent concurrent drawing to avoid marker duplication
    if (isDrawing) {
        qDebug() << "BTWGraph: draw() already in progress, skipping";
        return;
    }
    
    isDrawing = true;

    // Clear existing items - ensure complete clearing before drawing
    // Automatic circle markers are in graphicsScene, so clearing graphicsScene removes them
    graphicsScene->clear();
    graphicsScene->update(); // Force immediate update to ensure clearing is visible
    
    // Clear stored timestamps when redrawing (markers will be recreated)
    m_automaticMarkerTimestamps.clear();
    
    setupDrawingArea();

    if (gridEnabled)
    {
        drawGrid();
    }

    if (dataSource && !dataSource->isEmpty())
    {
        updateDataRanges();
        
        // Draw scatterplots for each series with their respective colors
        std::vector<QString> seriesLabels = dataSource->getDataSeriesLabels();
        for (const QString &seriesLabel : seriesLabels)
        {
            if (isSeriesVisible(seriesLabel))
            {
                QColor seriesColor = getSeriesColor(seriesLabel);
                
                if (seriesLabel == "ADOPTED")
                {
                    // Draw curve for ADOPTED series without points
                    drawDataLine(seriesLabel, false);
                }
                else
                {
                    // Draw scatterplot for other series
                    drawScatterplot(seriesLabel, seriesColor, 3.0, Qt::black);
                }

                // BTW markers are now manually placed through data source - no automatic generation
            }
        }
    }
    
    // Draw BTW symbols (magenta circles from other graphs)
    drawBTWSymbols();
    
    // Sync markers from data source to marker system
    syncMarkersFromDataSource();
    
    isDrawing = false;
}

/**
 * @brief Handle mouse click events specific to BTW graph
 * Adds a single interactive marker when clicking on the graph
 * Removes any existing marker before adding a new one
 *
 * @param scenePos Scene position of the click
 */
void BTWGraph::onMouseClick(const QPointF &scenePos)
{
    qDebug() << "BTWGraph mouse clicked at scene position:" << scenePos;
    
    // Check if we clicked on an existing interactive marker in the overlay scene
    if (overlayScene) {
        QGraphicsItem *itemAtPos = overlayScene->itemAt(scenePos, QTransform());
        // Filter out crosshair items - they should not prevent marker creation
        if (itemAtPos && itemAtPos != crosshairHorizontal && itemAtPos != crosshairVertical) {
            // Check if this is an InteractiveGraphicsItem from one of our markers
            InteractiveGraphicsItem *interactiveItem = qgraphicsitem_cast<InteractiveGraphicsItem*>(itemAtPos);
            if (interactiveItem) {
                qDebug() << "BTWGraph: Clicked on existing interactive marker, letting it handle the event";
                // Don't add a new marker, let the interactive item handle the click
                return;
            }
        }
    }
    
    // Only add a marker if we clicked on empty space (no interactive items)
    if (overlayScene) {
        // Calculate timestamp from Y position (this represents the time at that position on the graph)
        QDateTime timestamp = mapScreenToTime(scenePos.y());
        
        // If timestamp is invalid, fallback to current time
        if (!timestamp.isValid()) {
            timestamp = QDateTime::currentDateTime();
            qDebug() << "BTWGraph: Could not map Y position to timestamp, using current time";
        }
        
        // Calculate value (range) from X position
        qreal value = mapScreenXToRange(scenePos.x());
        
        // Create InteractiveBTWMarker directly - it will be drawn by WaterfallGraph
        auto marker = std::make_shared<InteractiveBTWMarker>(timestamp, value, scenePos, overlayScene);
        addMarker(marker);
        
        // Emit signal for external integration
        emit manualMarkerPlaced(timestamp, scenePos);
        
        qDebug() << "BTWGraph: Added new interactive marker at:" << scenePos << "with timestamp:" << timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz") << "value:" << value;
    }
    
    // Call parent implementation
    WaterfallGraph::onMouseClick(scenePos);
}

/**
 * @brief Handle mouse drag events specific to BTW graph
 *
 * @param scenePos Scene position of the drag
 */
void BTWGraph::onMouseDrag(const QPointF &scenePos)
{
    qDebug() << "BTWGraph mouse dragged to scene position:" << scenePos;
    // Call parent implementation
    WaterfallGraph::onMouseDrag(scenePos);
}

/**
 * @brief Draw BTW-specific scatterplot
 *
 */
void BTWGraph::drawBTWScatterplot()
{
    // By default, create a scatterplot using the parent's scatterplot functionality
    drawScatterplot(QString("BTW-1"), Qt::red, 4.0, Qt::white);

    qDebug() << "BTW scatterplot drawn";
}

/**
 * @brief Sync markers from sync state to the unified marker system
 */
void BTWGraph::syncMarkersFromDataSource()
{
    // Get sync state instead of data source
    GraphContainerSyncState *syncState = getSyncState();
    if (!syncState || !syncState->hasBTWMarkers) {
        return;
    }

    // Clear existing BTW circle markers
    QString circleMarkerType = QString::number(static_cast<int>(WaterfallMarker::MarkerType::BTWCircleMarker));
    
    // Remove existing BTW circle markers (make a copy of the vector to avoid iterator invalidation)
    auto markerHashes = m_markersByType[circleMarkerType];
    for (const QString& hash : markerHashes) {
        removeMarker(hash);
    }

    // Add BTW circle markers from sync state
    std::vector<BTWMarkerData> btwMarkers = syncState->btwMarkers;
    for (const auto& markerData : btwMarkers) {
        auto marker = std::make_shared<BTWCircleMarker>(markerData.timestamp, markerData.range, markerData.delta);
        addMarker(marker);
    }
    
    qDebug() << "BTW: Synced" << btwMarkers.size() << "circle markers from sync state";
}

/**
 * @brief Draw manually placed BTW circle markers from data source (DEPRECATED - use syncMarkersFromDataSource)
 * Circle outline with a line whose angle is calculated from delta values
 */
void BTWGraph::drawCustomCircleMarkers()
{
    if (!dataSource || !graphicsScene) {
        qDebug() << "BTW: drawCustomCircleMarkers early return - no dataSource or graphicsScene";
        return;
    }

    // Get manually placed markers from data source
    std::vector<BTWMarkerData> btwMarkers = dataSource->getBTWMarkers();
    
    if (btwMarkers.empty()) {
        qDebug() << "BTW: No manually placed markers in data source";
        return;
    }

    // Filter markers to only include those within the visible time range
    std::vector<BTWMarkerData> visibleMarkers;
    bool timeRangeValid = timeMin.isValid() && timeMax.isValid() && timeMin <= timeMax;
    
    if (timeRangeValid) {
        for (const auto& markerData : btwMarkers) {
            if (markerData.timestamp >= timeMin && markerData.timestamp <= timeMax) {
                visibleMarkers.push_back(markerData);
            }
        }
    } else {
        visibleMarkers = btwMarkers;
    }

    if (visibleMarkers.empty()) {
        qDebug() << "BTW: No visible markers within time range";
        return;
    }

    // Draw circle markers for each visible marker
    int markersDrawn = 0;
    qDebug() << "BTW: Drawing" << visibleMarkers.size() << "manually placed markers";
    
    for (const auto& markerData : visibleMarkers) {
        QDateTime timestamp = markerData.timestamp;
        qreal range = markerData.range;
        qreal deltaValue = markerData.delta;
        
        QPointF screenPos = mapDataToScreen(range, timestamp);
        
        // Check if point is within visible area
        if (drawingArea.contains(screenPos)) {
            // Calculate marker size based on window size
            QSize windowSize = this->size();
            qreal markerRadius = std::min(0.04 * windowSize.width(), 12.0); // Circle radius, cap at 12 pixels
            
            // Draw circle outline
            QGraphicsEllipseItem *circleOutline = new QGraphicsEllipseItem();
            circleOutline->setRect(screenPos.x() - markerRadius, screenPos.y() - markerRadius, 
                                 2 * markerRadius, 2 * markerRadius);
            circleOutline->setPen(QPen(Qt::blue, 2));
            circleOutline->setBrush(QBrush(Qt::transparent));
            circleOutline->setZValue(1000);
            
            graphicsScene->addItem(circleOutline);
            
            // Draw angled line (5x radius on both sides)
            qreal lineLength = 5 * markerRadius;
            
            // Calculate angle from delta value
            // Map delta value to angle: positive delta = positive angle (clockwise), negative delta = negative angle (counterclockwise)
            qreal angleDegrees = deltaValue * 10.0; // Scale factor to convert delta to meaningful angle
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
            
            graphicsScene->addItem(angledLine);
            
            // Add blue text label with rectangular outline beside the marker
            QString prefix = (deltaValue >= 0) ? "R" : "L";
            QString displayValue = (deltaValue >= 0) ? QString::number(deltaValue, 'f', 1) : QString::number(-deltaValue, 'f', 1);
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
            
            graphicsScene->addItem(textLabel);
            
            // Add rectangular outline around the text
            QGraphicsRectItem *textOutline = new QGraphicsRectItem();
            textOutline->setRect(textLabel->pos().x() - 2, textLabel->pos().y() - 2,
                               textRect.width() + 4, textRect.height() + 4);
            textOutline->setPen(QPen(Qt::blue, 1));
            textOutline->setBrush(QBrush(Qt::transparent));
            textOutline->setZValue(1001);
            
            graphicsScene->addItem(textOutline);
            
            markersDrawn++;
        }
    }
    
    qDebug() << "BTW: Drew" << markersDrawn << "manually placed circle markers";
}


void BTWGraph::deleteInteractiveMarkers()
{
    qDebug() << "BTWGraph: Clearing all interactive markers";
    
    // Remove all InteractiveBTWMarker instances from the unified marker system
    QString interactiveType = QString::number(static_cast<int>(WaterfallMarker::MarkerType::InteractiveBTWMarker));
    auto typeIt = m_markersByType.find(interactiveType);
    if (typeIt != m_markersByType.end()) {
        // Make a copy to avoid iterator invalidation
        auto hashes = typeIt->second;
        for (const QString& hash : hashes) {
            removeMarker(hash);
        }
    }
    
    qDebug() << "BTWGraph: Cleared all interactive markers from unified marker system";
}



std::vector<QDateTime> BTWGraph::getAutomaticMarkerTimestamps() const
{
    return m_automaticMarkerTimestamps;
}

void BTWGraph::addBTWSymbol(const QString &symbolName, const QDateTime &timestamp, qreal range)
{
    // Store symbol in dataSource (WaterfallData) so it persists with track data
    if (!dataSource)
    {
        qDebug() << "BTW: Cannot add symbol - no data source set";
        return;
    }
    
    dataSource->addBTWSymbol(symbolName, timestamp, range);
    
    qDebug() << "BTW: Added symbol" << symbolName << "at timestamp" << timestamp.toString() << "with range" << range << "to data source";
    
    // Trigger redraw
    draw();
}

BTWSymbolDrawing::SymbolType BTWGraph::symbolNameToType(const QString &symbolName) const
{
    QString name = symbolName.toUpper();
    if (name == "MAGENTACIRCLE") return BTWSymbolDrawing::SymbolType::MagentaCircle;
    
    // Default to MagentaCircle
    return BTWSymbolDrawing::SymbolType::MagentaCircle;
}

void BTWGraph::drawBTWSymbols()
{
    // Follow the same pattern as RTW symbols - read symbols from dataSource
    if (!graphicsScene || !dataSource)
    {
        return;
    }
    
    // Get symbols from dataSource
    std::vector<BTWSymbolData> btwSymbols = dataSource->getBTWSymbols();
    
    if (btwSymbols.empty())
    {
        return;
    }
    
    // Filter symbols to only include those within the visible time range
    std::vector<BTWSymbolData> visibleSymbols;
    bool timeRangeValid = timeMin.isValid() && timeMax.isValid() && timeMin <= timeMax;
    
    if (timeRangeValid)
    {
        for (const auto& symbolData : btwSymbols)
        {
            if (symbolData.timestamp >= timeMin && symbolData.timestamp <= timeMax)
            {
                visibleSymbols.push_back(symbolData);
            }
        }
    }
    else
    {
        visibleSymbols = btwSymbols;
    }
    
    // Draw symbols
    for (const auto& symbolData : visibleSymbols)
    {
        // Map symbol position to screen coordinates
        QPointF screenPos = mapDataToScreen(symbolData.range, symbolData.timestamp);
        
        // Check if point is within visible area
        if (!drawingArea.contains(screenPos))
        {
            continue;
        }
        
        // Convert symbol name to SymbolType
        BTWSymbolDrawing::SymbolType symbolType = symbolNameToType(symbolData.symbolName);
        
        // Get the pixmap for this symbol type
        const QPixmap& symbolPixmap = symbols.get(symbolType);
        
        // Validate pixmap before using it
        if (symbolPixmap.isNull() || symbolPixmap.width() <= 0 || symbolPixmap.height() <= 0)
        {
            continue;
        }
        
        // Create a graphics pixmap item and add it to the scene
        QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(symbolPixmap);
        
        // Center the symbol on the data point
        QRectF pixmapRect = pixmapItem->boundingRect();
        pixmapItem->setPos(screenPos.x() - pixmapRect.width()/2, screenPos.y() - pixmapRect.height()/2);
        pixmapItem->setZValue(1003); // Above markers but below interactive items
        
        graphicsScene->addItem(pixmapItem);
    }
}

void BTWGraph::addBTWSymbolToOtherGraphs(const QDateTime &timestamp, qreal btwValue)
{
    // Find parent GraphContainer to access GraphLayout
    QWidget *parent = this->parentWidget();
    if (!parent) return;
    
    // Try to find GraphContainer
    GraphContainer *container = qobject_cast<GraphContainer*>(parent);
    if (!container) return;
    
    // Try to find GraphLayout (parent of GraphContainer)
    QWidget *layoutWidget = container->parentWidget();
    if (!layoutWidget) return;
    
    GraphLayout *layout = qobject_cast<GraphLayout*>(layoutWidget);
    if (!layout) return;
    
    // Get all graph containers in the layout
    // We need to access m_graphContainers, but it's private, so we'll use a different approach
    // Get all containers using findChildren
    QList<GraphContainer*> allContainers = layout->findChildren<GraphContainer*>();
    
    for (GraphContainer *otherContainer : allContainers)
    {
        if (otherContainer == container) continue; // Skip self
        
        // Get all graph types in this container
        std::vector<GraphType> graphTypes = getAllGraphTypes();
        
        for (GraphType graphType : graphTypes)
        {
            if (graphType == GraphType::BTW) continue; // Skip BTW graphs
            
            // Check if this container has this graph type
            if (!otherContainer->hasDataOption(graphType)) continue;
            
            // Get the data source for this graph type
            WaterfallData *dataSource = layout->getDataSource(graphType);
            if (!dataSource) continue;
            
            // Check if symbol already exists at this timestamp (deduplication)
            // This prevents adding duplicate symbols when draw() is called multiple times
            std::vector<BTWSymbolData> existingSymbols = dataSource->getBTWSymbols();
            bool symbolExists = false;
            for (const auto& existingSymbol : existingSymbols)
            {
                // Check if symbol exists at the same timestamp (within 100ms tolerance)
                qint64 timeDiff = qAbs(existingSymbol.timestamp.msecsTo(timestamp));
                if (timeDiff < 100 && existingSymbol.symbolName == "MagentaCircle")
                {
                    symbolExists = true;
                    break;
                }
            }
            
            if (symbolExists) continue; // Skip if symbol already exists
            
            // Check if there's a datapoint at this timestamp
            bool hasDataPoint = false;
            qreal dataValue = 0.0;
            
            // Check all series in the data source
            std::vector<QString> seriesLabels = dataSource->getDataSeriesLabels();
            for (const QString &seriesLabel : seriesLabels)
            {
                // Get data points near this timestamp (within 1 second)
                const std::vector<QDateTime> &timestamps = dataSource->getTimestampsSeries(seriesLabel);
                const std::vector<qreal> &yData = dataSource->getYDataSeries(seriesLabel);
                
                for (size_t i = 0; i < timestamps.size(); ++i)
                {
                    qint64 timeDiff = qAbs(timestamps[i].msecsTo(timestamp));
                    if (timeDiff < 1000) // Within 1 second
                    {
                        hasDataPoint = true;
                        dataValue = yData[i];
                        break;
                    }
                }
                if (hasDataPoint) break;
            }
            
            // Only add symbol if there's a datapoint
            if (hasDataPoint)
            {
                // Add magenta circle symbol to this graph's data source
                dataSource->addBTWSymbol("MagentaCircle", timestamp, dataValue);
                
                // Trigger redraw of the graph
                WaterfallGraph *graph = otherContainer->getCurrentWaterfallGraph();
                if (graph && graph->getDataSource() == dataSource)
                {
                    graph->draw();
                }
            }
        }
    }
}
