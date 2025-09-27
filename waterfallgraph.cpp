#include "waterfallgraph.h"

/**
 * @brief Construct a new WaterfallGraph::WaterfallGraph object
 * 
 * @param parent Parent widget
 * @param enableGrid Whether to enable grid display
 * @param gridDivisions Number of grid divisions
 * @param timeInterval Time interval for the waterfall display
 */
WaterfallGraph::WaterfallGraph(QWidget *parent, bool enableGrid, int gridDivisions, TimeInterval timeInterval)
    : QWidget(parent)
    , graphicsView(nullptr)
    , graphicsScene(nullptr)
    , gridEnabled(enableGrid)
    , gridDivisions(gridDivisions)
    , yMin(0.0), yMax(0.0)
    , timeMin(QDateTime())
    , timeMax(QDateTime())
    , dataRangesValid(false)
    , rangeLimitingEnabled(false)
    , customYMin(0.0), customYMax(0.0)
    , timeInterval(timeInterval)
    , dataSource(nullptr)
    , isDragging(false)
    , mouseSelectionEnabled(false)
    , selectionRect(nullptr)
{
    // Remove all margins and padding for snug fit
    setContentsMargins(0, 0, 0, 0);
    
    // Set black background
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);
    
    // Ensure the widget expands to fill all available space in the parent container
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Initialize scene
    graphicsScene = new QGraphicsScene(this);
    graphicsScene->setBackgroundBrush(QBrush(Qt::black));
    
    // Create graphics view
    graphicsView = new QGraphicsView(graphicsScene, this);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setDragMode(QGraphicsView::NoDrag); // We'll handle our own mouse events
    graphicsView->setMouseTracking(true); // Enable mouse tracking
    graphicsView->setAttribute(Qt::WA_TransparentForMouseEvents, true); // Make transparent to mouse events
    
    // Set black background for view
    graphicsView->setBackgroundBrush(QBrush(Qt::black));
    
    // Disable scrollbars
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Ensure the view fits the scene exactly
    graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    graphicsView->setFrameStyle(QFrame::NoFrame);
    
    // Set size policy for graphics view to fill the widget
    graphicsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Set up layout to make the graphics view fill the widget with no margins
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(graphicsView);
    setLayout(layout);
    
    // Size policy will be set by parent container
    
    // Enable mouse tracking
    setMouseTracking(true);
    
    // Debug: Print initial state
    qDebug() << "WaterfallGraph constructor - mouseSelectionEnabled:" << mouseSelectionEnabled;
    qDebug() << "WaterfallGraph constructor - graphicsScene:" << graphicsScene;
    qDebug() << "WaterfallGraph constructor - graphicsView:" << graphicsView;
    
    // Initial setup will happen in showEvent
    qDebug() << "Constructor - Widget size:" << this->size();
}

/**
 * @brief Destroy the WaterfallGraph::WaterfallGraph object
 * 
 */
WaterfallGraph::~WaterfallGraph()
{
    // Note: graphicsView and graphicsScene are child widgets/scenes, so they will be automatically deleted by Qt's parent-child mechanism
}

/**
 * @brief Set the data source for this waterfall graph.
 * 
 * @param dataSource Reference to the WaterfallData object to use
 */
void WaterfallGraph::setDataSource(WaterfallData& dataSource)
{
    this->dataSource = &dataSource;
    draw(); // Trigger redraw with new data source
    qDebug() << "Data source set successfully";
}

/**
 * @brief Get the current data source.
 * 
 * @return WaterfallData* Pointer to the current data source, or nullptr if not set
 */
WaterfallData* WaterfallGraph::getDataSource() const
{
    return dataSource;
}

/**
 * @brief Set the data for the graph.
 * 
 * @param yData 
 * @param timestamps 
 */
void WaterfallGraph::setData(const std::vector<qreal>& yData, const std::vector<QDateTime>& timestamps)
{
    if (!dataSource) {
        qDebug() << "Error: No data source set";
        return;
    }
    
    // Store the data using the data source
    dataSource->setData(yData, timestamps);
    
    qDebug() << "Data set successfully. Size:" << dataSource->getDataSize();
    
    // Redraw the graph with the new data
    draw();
}

/**
 * @brief Set the data for the graph using WaterfallData object.
 * 
 * @param data 
 */
void WaterfallGraph::setData(const WaterfallData& data)
{
    if (!dataSource) {
        qDebug() << "Error: No data source set";
        return;
    }
    
    *dataSource = data;
    
    qDebug() << "Data set successfully from WaterfallData object. Size:" << dataSource->getDataSize();
    
    // Redraw the graph with the new data
    draw();
}

/**
 * @brief Clear all data from the graph.
 * 
 */
void WaterfallGraph::clearData()
{
    if (!dataSource) {
        qDebug() << "Error: No data source set";
        return;
    }
    
    dataSource->clearData();
    
    qDebug() << "Data cleared successfully";
    
    // Redraw the graph
    draw();
}

/**
 * @brief Add a single data point to the graph.
 * 
 * @param yValue 
 * @param timestamp 
 */
void WaterfallGraph::addDataPoint(qreal yValue, const QDateTime& timestamp)
{
    if (!dataSource) {
        qDebug() << "Error: No data source set";
        return;
    }
    
    dataSource->addDataPoint(yValue, timestamp);
    
    qDebug() << "Data point added. New size:" << dataSource->getDataSize();
    
    // Redraw the graph with the new data
    draw();
}

/**
 * @brief Add multiple data points to the graph.
 * 
 * @param yValues 
 * @param timestamps 
 */
void WaterfallGraph::addDataPoints(const std::vector<qreal>& yValues, const std::vector<QDateTime>& timestamps)
{
    if (!dataSource) {
        qDebug() << "Error: No data source set";
        return;
    }
    
    dataSource->addDataPoints(yValues, timestamps);
    
    qDebug() << "Data points added. New size:" << dataSource->getDataSize();
    
    // Redraw the graph with the new data
    draw();
}

/**
 * @brief Get all data from the graph.
 * 
 * @return WaterfallData 
 */
WaterfallData WaterfallGraph::getData() const
{
    if (!dataSource) {
        return WaterfallData();
    }
    return *dataSource;
}

/**
 * @brief Get data within specified y extents.
 * 
 * @param yMin 
 * @param yMax 
 * @return std::vector<std::pair<qreal, QDateTime>> 
 */
std::vector<std::pair<qreal, QDateTime>> WaterfallGraph::getDataWithinYExtents(qreal yMin, qreal yMax) const
{
    if (!dataSource) {
        return std::vector<std::pair<qreal, QDateTime>>();
    }
    return dataSource->getDataWithinYExtents(yMin, yMax);
}

/**
 * @brief Get data within specified time range.
 * 
 * @param startTime 
 * @param endTime 
 * @return std::vector<std::pair<qreal, QDateTime>> 
 */
std::vector<std::pair<qreal, QDateTime>> WaterfallGraph::getDataWithinTimeRange(const QDateTime& startTime, const QDateTime& endTime) const
{
    if (!dataSource) {
        return std::vector<std::pair<qreal, QDateTime>>();
    }
    return dataSource->getDataWithinTimeRange(startTime, endTime);
}

/**
 * @brief Get direct access to y data vector.
 * 
 * @return const std::vector<qreal>& 
 */
const std::vector<qreal>& WaterfallGraph::getYData() const
{
    static const std::vector<qreal> emptyVector;
    if (!dataSource) {
        return emptyVector;
    }
    return dataSource->getYData();
}

/**
 * @brief Get direct access to timestamps vector.
 * 
 * @return const std::vector<QDateTime>& 
 */
const std::vector<QDateTime>& WaterfallGraph::getTimestamps() const
{
    static const std::vector<QDateTime> emptyVector;
    if (!dataSource) {
        return emptyVector;
    }
    return dataSource->getTimestamps();
}

/**
 * @brief Set the time interval for the waterfall graph.
 * 
 * @param interval Time interval enum value
 */
void WaterfallGraph::setTimeInterval(TimeInterval interval)
{
    timeInterval = interval;
    
    // Always update time range based on new interval
    timeMax = QDateTime::currentDateTime(); // Current time (top of graph)
    timeMin = timeMax.addMSecs(-getTimeIntervalMs()); // Bottom of graph
    
    // Update data ranges if we have data
    if (dataSource && !dataSource->isEmpty()) {
        auto yRange = dataSource->getYRange();
        yMin = yRange.first;
        yMax = yRange.second;
        dataRangesValid = true;
    } else {
        // Set default Y range when no data
        yMin = 0.0;
        yMax = 100.0;
        dataRangesValid = true;
    }
    
    // Force redraw regardless of data presence to update grid and layout
    draw();
    
    qDebug() << "Time interval set to:" << timeIntervalToString(interval);
}

/**
 * @brief Get the current time interval.
 * 
 * @return TimeInterval Current time interval enum value
 */
TimeInterval WaterfallGraph::getTimeInterval() const
{
    return timeInterval;
}

/**
 * @brief Get the current time interval in milliseconds.
 * 
 * @return qint64 Time interval in milliseconds
 */
qint64 WaterfallGraph::getTimeIntervalMs() const
{
    return static_cast<qint64>(timeInterval) * 60 * 1000; // Convert minutes to milliseconds
}

/**
 * @brief Enable or disable the grid display.
 * 
 * @param enabled True to enable grid, false to disable
 */
void WaterfallGraph::setGridEnabled(bool enabled)
{
    if (gridEnabled != enabled) {
        gridEnabled = enabled;
        draw(); // Redraw to show/hide grid
        qDebug() << "Grid" << (enabled ? "enabled" : "disabled");
    }
}

/**
 * @brief Check if the grid is currently enabled.
 * 
 * @return bool True if grid is enabled, false otherwise
 */
bool WaterfallGraph::isGridEnabled() const
{
    return gridEnabled;
}

/**
 * @brief Set the number of grid divisions.
 * 
 * @param divisions Number of grid divisions (must be positive)
 */
void WaterfallGraph::setGridDivisions(int divisions)
{
    if (divisions > 0 && gridDivisions != divisions) {
        gridDivisions = divisions;
        if (gridEnabled) {
            draw(); // Redraw to update grid divisions
        }
        qDebug() << "Grid divisions set to:" << divisions;
    }
}

/**
 * @brief Get the current number of grid divisions.
 * 
 * @return int Number of grid divisions
 */
int WaterfallGraph::getGridDivisions() const
{
    return gridDivisions;
}

/**
 * @brief Handle mouse click events.
 * 
 * @param scenePos 
 */
void WaterfallGraph::onMouseClick(const QPointF& scenePos)
{
    qDebug() << "Mouse clicked at scene position:" << scenePos;
    // This is a virtual function that can be overridden in derived classes
}

void WaterfallGraph::onMouseDrag(const QPointF& scenePos)
{
    qDebug() << "Mouse dragged to scene position:" << scenePos;
    // This is a virtual function that can be overridden in derived classes
}

/**
 * @brief Draw the graph.
 * 
 */
void WaterfallGraph::draw()
{
    if (!graphicsScene)
        return;

    // Clear existing items
    graphicsScene->clear();

    // Update the drawing area
    setupDrawingArea();

    // Draw grid if enabled
    if (gridEnabled) {
        drawGrid();
    }

    // Draw the actual data line if we have data
    if (dataSource && !dataSource->isEmpty()) {
        updateDataRanges();
        drawDataLine();
    }
}

/**
 * @brief Update the graphics dimensions.
 * 
 */
void WaterfallGraph::updateGraphicsDimensions()
{
    if (!graphicsView || !graphicsScene) return;
    
    // Get the current size of the widget
    QSize widgetSize = this->size();
    
    qDebug() << "updateGraphicsDimensions - Widget size:" << widgetSize;
    
    // Only update if we have valid dimensions
    if (widgetSize.width() > 0 && widgetSize.height() > 0) {
        // Set scene rect to match widget size exactly
        QRectF newSceneRect(0, 0, widgetSize.width(), widgetSize.height());
        graphicsScene->setSceneRect(newSceneRect);
        
        // Ensure the graphics view fits the scene exactly (no scrollbars)
        graphicsView->fitInView(newSceneRect, Qt::KeepAspectRatio);
        graphicsView->resetTransform(); // Reset any scaling
        graphicsView->setTransform(QTransform()); // Ensure 1:1 mapping
        
        // Update the drawing area
        setupDrawingArea();
        
        // Redraw the scene
        draw();
        
        qDebug() << "Graphics dimensions updated successfully to:" << widgetSize;
        qDebug() << "Scene rect is now:" << graphicsScene->sceneRect();
    } else {
        qDebug() << "Widget size is invalid, skipping update";
    }
}

/**
 * @brief Setup the drawing area.
 * 
 */
void WaterfallGraph::setupDrawingArea()
{
    // Set up the drawing area to cover the entire scene
    drawingArea = graphicsScene->sceneRect();
    qDebug() << "Drawing area set to:" << drawingArea;
}


/**
 * @brief Draw the grid.
 * 
 */
void WaterfallGraph::drawGrid()
{
    if (!graphicsScene || !gridEnabled || drawingArea.isEmpty() || gridDivisions <= 0) return;
    
    QPen gridPen(Qt::white, 1, Qt::DashLine); // White grid lines for black background
    
    // Draw vertical grid lines (for x-axis - variable)
    double stepX = drawingArea.width() / gridDivisions;
    for (int i = 0; i <= gridDivisions; ++i) {
        double x = drawingArea.left() + i * stepX;
        graphicsScene->addLine(x, drawingArea.top(), x, drawingArea.bottom(), gridPen);
    }
    
    // Draw horizontal grid lines (for y-axis - time)
    // These represent time divisions within the fixed interval
    double stepY = drawingArea.height() / gridDivisions;
    for (int i = 0; i <= gridDivisions; ++i) {
        double y = drawingArea.top() + i * stepY;
        graphicsScene->addLine(drawingArea.left(), y, drawingArea.right(), y, gridPen);
    }
    
    // Draw border
    QPen borderPen(Qt::white, 2); // White border for black background
    graphicsScene->addRect(drawingArea, borderPen);
}

/**
 * @brief Handle mouse press events.
 * 
 * @param event 
 */
void WaterfallGraph::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse press event - button:" << event->button() << "mouseSelectionEnabled:" << mouseSelectionEnabled;
    
    if (event->button() == Qt::LeftButton) {
        // Convert widget coordinates to scene coordinates
        QPointF scenePos = graphicsView->mapToScene(event->pos());
        qDebug() << "Scene position:" << scenePos << "drawingArea:" << drawingArea;
        
        // Check if the click is within the drawing area
        if (drawingArea.contains(scenePos)) {
            isDragging = true;
            lastMousePos = scenePos;
            
            // Start selection if mouse selection is enabled
            if (mouseSelectionEnabled) {
                qDebug() << "Starting selection...";
                startSelection(scenePos);
            } else {
                qDebug() << "Mouse selection is disabled";
            }
            
            onMouseClick(scenePos);
        } else {
            qDebug() << "Click outside drawing area";
        }
    }
    
    // Call parent implementation
    QWidget::mousePressEvent(event);
}

/**
 * @brief Handle mouse move events.
 * 
 * @param event 
 */
void WaterfallGraph::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        // Convert widget coordinates to scene coordinates
        QPointF scenePos = graphicsView->mapToScene(event->pos());
        
        // Check if the move is within the drawing area
        if (drawingArea.contains(scenePos)) {
            // Update selection if mouse selection is enabled
            if (mouseSelectionEnabled) {
                updateSelection(scenePos);
            }
            
            onMouseDrag(scenePos);
            lastMousePos = scenePos;
        }
    }
    
    // Call parent implementation
    QWidget::mouseMoveEvent(event);
}

/**
 * @brief Handle mouse release events.
 * 
 * @param event 
 */
void WaterfallGraph::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // End selection if mouse selection is enabled
        if (mouseSelectionEnabled) {
            endSelection();
        }
        
        isDragging = false;
    }
    
    // Call parent implementation
    QWidget::mouseReleaseEvent(event);
}

/**
 * @brief Handle resize events.
 * 
 * @param event 
 */
void WaterfallGraph::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // Ensure graphics view fits the widget exactly
    if (graphicsView) {
        graphicsView->resize(event->size());
    }
    
    // Update graphics dimensions when the widget is resized
    updateGraphicsDimensions();
    
    qDebug() << "Resize event - New size:" << size();
}

/**
 * @brief Handle show events.
 * 
 * @param event 
 */
void WaterfallGraph::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    // This is called when the widget becomes visible
    qDebug() << "showEvent - Widget size:" << this->size();
    qDebug() << "showEvent - Graphics view size:" << graphicsView->size();
    
    // Ensure graphics view fits the widget exactly
    if (graphicsView) {
        graphicsView->resize(this->size());
    }
    
    // Update graphics dimensions now that we're visible
    updateGraphicsDimensions();
}

/**
 * @brief Update data ranges from the waterfall data.
 * 
 */
void WaterfallGraph::updateDataRanges()
{
    if (!dataSource || dataSource->isEmpty()) {
        dataRangesValid = false;
        return;
    }
    
    auto yRange = dataSource->getYRange();
    qreal dataYMin = yRange.first;
    qreal dataYMax = yRange.second;
    
    if (rangeLimitingEnabled) {
        // Apply range limiting: use the intersection of custom range and data range
        // Custom range acts as bounds, but we never exceed the actual data range
        yMin = qMax(customYMin, dataYMin);  // Take the larger of custom min and data min
        yMax = qMin(customYMax, dataYMax);  // Take the smaller of custom max and data max
        
        // Ensure min < max
        if (yMin >= yMax) {
            // If custom range is invalid or doesn't overlap with data, use data range
            yMin = dataYMin;
            yMax = dataYMax;
            qDebug() << "Warning: Custom range doesn't overlap with data range, using data range";
        }
    } else {
        // Use data range directly
        yMin = dataYMin;
        yMax = dataYMax;
    }
    
    // Set time range based on fixed interval with current time as top (t=0)
    timeMax = QDateTime::currentDateTime(); // Current time (top of graph)
    timeMin = timeMax.addMSecs(-getTimeIntervalMs()); // Bottom of graph
    
    dataRangesValid = true;
    
    qDebug() << "Data ranges updated - Y:" << yMin << "to" << yMax 
             << "Time:" << timeMin.toString() << "to" << timeMax.toString()
             << "Interval:" << timeIntervalToString(timeInterval)
             << "Range limiting:" << (rangeLimitingEnabled ? "enabled" : "disabled");
}

/**
 * @brief Map data coordinates to screen coordinates.
 * 
 * @param yValue 
 * @param timestamp 
 * @return QPointF 
 */
QPointF WaterfallGraph::mapDataToScreen(qreal yValue, const QDateTime& timestamp) const
{
    if (!dataRangesValid || drawingArea.isEmpty()) {
        return QPointF(0, 0);
    }
    
    // Map y-value to x-coordinate (horizontal position)
    qreal x = drawingArea.left() + ((yValue - yMin) / (yMax - yMin)) * drawingArea.width();
    
    // Map timestamp to y-coordinate (vertical position, top to bottom)
    // Use fixed time interval instead of data range
    qint64 timeOffset = timestamp.msecsTo(timeMax); // Time from current time (top) to data point
    qreal y = drawingArea.top() + (timeOffset / (qreal)getTimeIntervalMs()) * drawingArea.height();
    
    return QPointF(x, y);
}

/**
 * @brief Draw the data line from top to bottom.
 * 
 */
void WaterfallGraph::drawDataLine()
{
    if (!graphicsScene || !dataSource || dataSource->isEmpty() || !dataRangesValid) {
        return;
    }
    
    const auto& yData = dataSource->getYData();
    const auto& timestamps = dataSource->getTimestamps();
    
    // Filter data points to only include those within the current time range
    std::vector<std::pair<qreal, QDateTime>> visibleData;
    for (size_t i = 0; i < yData.size(); ++i) {
        if (timestamps[i] >= timeMin && timestamps[i] <= timeMax) {
            visibleData.push_back({yData[i], timestamps[i]});
        }
    }
    
    if (visibleData.empty()) {
        qDebug() << "No data points within current time range";
        return;
    }
    
    if (visibleData.size() < 2) {
        // Draw a single point if we only have one data point
        QPointF screenPoint = mapDataToScreen(visibleData[0].first, visibleData[0].second);
        QPen pointPen(Qt::green, 3);
        graphicsScene->addEllipse(screenPoint.x() - 2, screenPoint.y() - 2, 4, 4, pointPen);
        qDebug() << "Data line drawn with 1 visible point";
        return;
    }
    
    // Create a path for the line
    QPainterPath path;
    QPointF firstPoint = mapDataToScreen(visibleData[0].first, visibleData[0].second);
    path.moveTo(firstPoint);
    
    // Add lines connecting all visible data points
    for (size_t i = 1; i < visibleData.size(); ++i) {
        QPointF point = mapDataToScreen(visibleData[i].first, visibleData[i].second);
        path.lineTo(point);
    }
    
    // Draw the line
    QPen linePen(Qt::green, 2);
    graphicsScene->addPath(path, linePen);
    
    // Draw data points
    QPen pointPen(Qt::yellow, 2);
    for (size_t i = 0; i < visibleData.size(); ++i) {
        QPointF point = mapDataToScreen(visibleData[i].first, visibleData[i].second);
        graphicsScene->addEllipse(point.x() - 1, point.y() - 1, 2, 2, pointPen);
    }
    
    qDebug() << "Data line drawn with" << visibleData.size() << "visible points out of" << yData.size() << "total points";
}

// Mouse selection functionality implementation

void WaterfallGraph::setMouseSelectionEnabled(bool enabled)
{
    mouseSelectionEnabled = enabled;
    if (!enabled) {
        clearSelection();
    }
    qDebug() << "Mouse selection" << (enabled ? "enabled" : "disabled");
}

bool WaterfallGraph::isMouseSelectionEnabled() const
{
    return mouseSelectionEnabled;
}

void WaterfallGraph::startSelection(const QPointF& scenePos)
{
    qDebug() << "startSelection called with scenePos:" << scenePos;
    qDebug() << "graphicsScene:" << graphicsScene;
    
    selectionStartPos = scenePos;
    selectionEndPos = scenePos;
    
    // Create selection rectangle
    if (selectionRect) {
        qDebug() << "Removing existing selection rectangle";
        graphicsScene->removeItem(selectionRect);
        delete selectionRect;
    }
    
    qDebug() << "Creating new selection rectangle";
    selectionRect = new QGraphicsRectItem();
    selectionRect->setPen(QPen(Qt::white, 2, Qt::DashLine)); // White dashed line
    selectionRect->setBrush(QBrush(QColor(255, 255, 255, 50))); // Semi-transparent white
    selectionRect->setZValue(1000); // Ensure it's drawn on top
    graphicsScene->addItem(selectionRect);
    
    // Initialize with a small rectangle at the start position
    selectionRect->setRect(scenePos.x() - 1, scenePos.y() - 1, 2, 2);
    
    qDebug() << "Selection rectangle created and added to scene. Rect:" << selectionRect->rect();
    qDebug() << "Selection started at:" << scenePos;
}

void WaterfallGraph::updateSelection(const QPointF& scenePos)
{
    if (!selectionRect) return;
    
    selectionEndPos = scenePos;
    
    // Update rectangle bounds
    QRectF rect;
    rect.setLeft(qMin(selectionStartPos.x(), selectionEndPos.x()));
    rect.setRight(qMax(selectionStartPos.x(), selectionEndPos.x()));
    rect.setTop(qMin(selectionStartPos.y(), selectionEndPos.y()));
    rect.setBottom(qMax(selectionStartPos.y(), selectionEndPos.y()));
    
    // Clamp to drawing area
    rect = rect.intersected(drawingArea);
    
    selectionRect->setRect(rect);
}

void WaterfallGraph::endSelection()
{
    if (!selectionRect) return;
    
    // Calculate the time range of the selection
    // Find min and max Y positions (min Y = later time, max Y = earlier time)
    qreal minY = qMin(selectionStartPos.y(), selectionEndPos.y());
    qreal maxY = qMax(selectionStartPos.y(), selectionEndPos.y());
    
    // Map to time: minY (bottom) = later time, maxY (top) = earlier time
    QTime startTime = mapScreenToTime(maxY); // Earlier time (top of selection)
    QTime endTime = mapScreenToTime(minY); // Later time (bottom of selection)
    
    qDebug() << "Selection Y range: minY=" << minY << "maxY=" << maxY;
    qDebug() << "Time range: start=" << startTime.toString() << "end=" << endTime.toString();
    
    // Only emit signal if selection has meaningful size
    if (startTime != endTime) {
        TimeSelectionSpan selection(startTime, endTime);
        emit SelectionCreated(selection);
        qDebug() << "Selection created:" << startTime.toString() << "to" << endTime.toString();
    } else {
        qDebug() << "Selection too small, ignoring";
    }
    
    // Clear the visual selection immediately on mouse release
    clearSelection();
}

void WaterfallGraph::clearSelection()
{
    if (selectionRect) {
        graphicsScene->removeItem(selectionRect);
        delete selectionRect;
        selectionRect = nullptr;
    }
}

QTime WaterfallGraph::mapScreenToTime(qreal yPos) const
{
    if (!dataRangesValid || drawingArea.isEmpty()) {
        return QTime();
    }
    
    // Map y-coordinate to time
    // yPos is from top (current time) to bottom (past time)
    qreal normalizedY = (yPos - drawingArea.top()) / drawingArea.height();
    normalizedY = qMax(0.0, qMin(1.0, normalizedY)); // Clamp to [0,1]
    
    // Calculate time offset from current time (top of graph)
    qint64 timeOffsetMs = static_cast<qint64>(normalizedY * getTimeIntervalMs());
    
    // Convert to QTime
    QDateTime selectionTime = timeMax.addMSecs(-timeOffsetMs);
    return selectionTime.time();
}

void WaterfallGraph::testSelectionRectangle()
{
    qDebug() << "testSelectionRectangle called";
    if (!graphicsScene) {
        qDebug() << "Graphics scene is null!";
        return;
    }
    
    // Create a test rectangle
    QGraphicsRectItem* testRect = new QGraphicsRectItem(100, 100, 200, 100);
    testRect->setPen(QPen(Qt::white, 2, Qt::DashLine));
    testRect->setBrush(QBrush(QColor(255, 255, 255, 50)));
    testRect->setZValue(1000);
    graphicsScene->addItem(testRect);
    
    qDebug() << "Test selection rectangle added to scene";
}

// Range limiting methods implementation

/**
 * @brief Enable or disable range limiting functionality.
 * 
 * @param enabled True to enable range limiting, false to disable
 */
void WaterfallGraph::setRangeLimitingEnabled(bool enabled)
{
    if (rangeLimitingEnabled != enabled) {
        rangeLimitingEnabled = enabled;
        
        // Update data ranges and redraw if we have data
        if (dataSource && !dataSource->isEmpty()) {
            updateDataRanges();
            draw();
        }
        
        qDebug() << "Range limiting" << (enabled ? "enabled" : "disabled");
    }
}

/**
 * @brief Check if range limiting is currently enabled.
 * 
 * @return bool True if range limiting is enabled, false otherwise
 */
bool WaterfallGraph::isRangeLimitingEnabled() const
{
    return rangeLimitingEnabled;
}

/**
 * @brief Set custom Y range for limiting the display range.
 * 
 * @param yMin Minimum Y value for the custom range
 * @param yMax Maximum Y value for the custom range
 */
void WaterfallGraph::setCustomYRange(qreal yMin, qreal yMax)
{
    // Validate the range
    if (yMin >= yMax) {
        qDebug() << "Error: Invalid custom Y range - min must be less than max";
        return;
    }
    
    customYMin = yMin;
    customYMax = yMax;
    
    // Update data ranges and redraw if range limiting is enabled and we have data
    if (rangeLimitingEnabled && dataSource && !dataSource->isEmpty()) {
        updateDataRanges();
        draw();
    }
    
    qDebug() << "Custom Y range set to:" << yMin << "to" << yMax;
}

/**
 * @brief Get the current custom Y range values.
 * 
 * @param yMin Reference to store the minimum Y value
 * @param yMax Reference to store the maximum Y value
 */
void WaterfallGraph::getCustomYRange(qreal& yMin, qreal& yMax) const
{
    yMin = customYMin;
    yMax = customYMax;
}

/**
 * @brief Update the time range and redraw the graph.
 * 
 */
void WaterfallGraph::updateTimeRange()
{
    // Update time range based on fixed interval with current time as top (t=0)
    timeMax = QDateTime::currentDateTime(); // Current time (top of graph)
    timeMin = timeMax.addMSecs(-getTimeIntervalMs()); // Bottom of graph
    
    // Update data ranges if we have data
    if (dataSource && !dataSource->isEmpty()) {
        updateDataRanges();
    }
    
    // Force redraw to show only data within the new time range
    draw();
    
    qDebug() << "Time range updated - Time:" << timeMin.toString() << "to" << timeMax.toString()
             << "Interval:" << timeIntervalToString(timeInterval);
}

/**
 * @brief Unset the custom Y range and revert to using the data's min/max range.
 * 
 */
void WaterfallGraph::unsetCustomYRange()
{
    customYMin = 0.0;
    customYMax = 0.0;
    
    // Update data ranges and redraw if range limiting is enabled and we have data
    if (rangeLimitingEnabled && dataSource && !dataSource->isEmpty()) {
        updateDataRanges();
        draw();
    }
    
    qDebug() << "Custom Y range unset, reverting to data range";
}

/**
 * @brief Draw a point as a small dot at the specified position.
 * 
 * @param position The position to draw the point at
 * @param color The color of the point (default: white)
 * @param size The size of the point (default: 2.0)
 */
void WaterfallGraph::drawPoint(const QPointF& position, const QColor& color, qreal size)
{
    if (!graphicsScene) return;
    
    // Create a small circle for the point
    QGraphicsEllipseItem* point = new QGraphicsEllipseItem();
    point->setRect(position.x() - size/2, position.y() - size/2, size, size);
    point->setPen(QPen(color, 1));
    point->setBrush(QBrush(color));
    point->setZValue(100); // Draw on top of grid but below selection
    
    graphicsScene->addItem(point);
}

/**
 * @brief Draw a dashed white translucent axis line between two points.
 * 
 * @param startPos The starting position of the line
 * @param endPos The ending position of the line
 * @param color The color of the line (default: white translucent)
 */
void WaterfallGraph::drawAxisLine(const QPointF& startPos, const QPointF& endPos, const QColor& color)
{
    if (!graphicsScene) return;
    
    // Create a dashed line
    QGraphicsLineItem* line = new QGraphicsLineItem();
    line->setLine(startPos.x(), startPos.y(), endPos.x(), endPos.y());
    line->setPen(QPen(color, 1, Qt::DashLine));
    line->setZValue(50); // Draw above grid but below data points
    
    graphicsScene->addItem(line);
}

/**
 * @brief Draw a character label at the specified position.
 * 
 * @param text The text to display
 * @param position The position to draw the text at
 * @param color The color of the text (default: white)
 * @param fontSize The font size (default: 12)
 */
void WaterfallGraph::drawCharacterLabel(const QString& text, const QPointF& position, const QColor& color, int fontSize)
{
    if (!graphicsScene) return;
    
    // Create a text item
    QGraphicsTextItem* textItem = new QGraphicsTextItem();
    textItem->setPlainText(text);
    textItem->setPos(position);
    
    // Set font and color
    QFont font;
    font.setPointSize(fontSize);
    textItem->setFont(font);
    textItem->setDefaultTextColor(color);
    textItem->setZValue(200); // Draw on top of everything
    
    graphicsScene->addItem(textItem);
}

/**
 * @brief Draw a solid triangle with square outline marker at the specified position.
 * 
 * @param position The position to draw the marker at
 * @param fillColor The fill color of the triangle (default: red)
 * @param outlineColor The outline color (default: black)
 * @param size The size of the marker (default: 8.0)
 */
void WaterfallGraph::drawTriangleMarker(const QPointF& position, const QColor& fillColor, const QColor& outlineColor, qreal size)
{
    if (!graphicsScene) return;
    
    // Create triangle polygon (pointing up)
    QPolygonF triangle;
    triangle << QPointF(position.x(), position.y() - size/2)           // Top point
             << QPointF(position.x() - size/2, position.y() + size/2)   // Bottom left
             << QPointF(position.x() + size/2, position.y() + size/2);  // Bottom right
    
    // Create the triangle polygon item
    QGraphicsPolygonItem* triangleItem = new QGraphicsPolygonItem(triangle);
    triangleItem->setPen(QPen(outlineColor, 2));
    triangleItem->setBrush(QBrush(fillColor));
    triangleItem->setZValue(150); // Draw above data points but below text
    
    graphicsScene->addItem(triangleItem);
    
    // Create square outline around the triangle
    QGraphicsRectItem* squareOutline = new QGraphicsRectItem();
    squareOutline->setRect(position.x() - size/2, position.y() - size/2, size, size);
    squareOutline->setPen(QPen(outlineColor, 2));
    squareOutline->setBrush(QBrush(Qt::transparent));
    squareOutline->setZValue(149); // Draw just below the triangle
    
    graphicsScene->addItem(squareOutline);
}

/**
 * @brief Draw a scatterplot for a given data series.
 * 
 * @param seriesLabel The label of the data series to plot
 * @param pointColor The color of the scatterplot points (default: white)
 * @param pointSize The size of the scatterplot points (default: 3.0)
 * @param outlineColor The outline color of the scatterplot points (default: black)
 */
void WaterfallGraph::drawScatterplot(const QString& seriesLabel, const QColor& pointColor, qreal pointSize, const QColor& outlineColor)
{
    if (!graphicsScene || !dataSource) return;
    
    // Get the data series
    const std::vector<qreal>& yData = dataSource->getYDataSeries(seriesLabel);
    const std::vector<QDateTime>& timestamps = dataSource->getTimestampsSeries(seriesLabel);
    
    if (yData.empty() || timestamps.empty()) {
        qDebug() << "No data available for scatterplot series:" << seriesLabel;
        return;
    }
    
    if (yData.size() != timestamps.size()) {
        qDebug() << "Data size mismatch for scatterplot series:" << seriesLabel;
        return;
    }
    
    // Filter data points to only include those within the current time range
    std::vector<std::pair<qreal, QDateTime>> visibleData;
    for (size_t i = 0; i < yData.size(); ++i) {
        if (timestamps[i] >= timeMin && timestamps[i] <= timeMax) {
            visibleData.push_back({yData[i], timestamps[i]});
        }
    }
    
    if (visibleData.empty()) {
        qDebug() << "No data points within current time range for scatterplot series:" << seriesLabel;
        return;
    }
    
    // Draw scatterplot points
    for (const auto& dataPoint : visibleData) {
        QPointF screenPoint = mapDataToScreen(dataPoint.first, dataPoint.second);
        
        // Create a circle for the scatterplot point
        QGraphicsEllipseItem* point = new QGraphicsEllipseItem();
        point->setRect(screenPoint.x() - pointSize/2, screenPoint.y() - pointSize/2, pointSize, pointSize);
        point->setPen(QPen(outlineColor, 1));
        point->setBrush(QBrush(pointColor));
        point->setZValue(120); // Draw above data lines but below markers
        
        graphicsScene->addItem(point);
    }
    
    qDebug() << "Scatterplot drawn for series" << seriesLabel << "with" << visibleData.size() << "points";
}

/**
 * @brief Draw a scatterplot for the default data series.
 * 
 * @param pointColor The color of the scatterplot points (default: white)
 * @param pointSize The size of the scatterplot points (default: 3.0)
 * @param outlineColor The outline color of the scatterplot points (default: black)
 */
void WaterfallGraph::drawScatterplot(const QColor& pointColor, qreal pointSize, const QColor& outlineColor)
{
    if (!graphicsScene || !dataSource) return;
    
    // Get the default data series
    const std::vector<qreal>& yData = dataSource->getYData();
    const std::vector<QDateTime>& timestamps = dataSource->getTimestamps();
    
    if (yData.empty() || timestamps.empty()) {
        qDebug() << "No data available for default scatterplot";
        return;
    }
    
    if (yData.size() != timestamps.size()) {
        qDebug() << "Data size mismatch for default scatterplot";
        return;
    }
    
    // Filter data points to only include those within the current time range
    std::vector<std::pair<qreal, QDateTime>> visibleData;
    for (size_t i = 0; i < yData.size(); ++i) {
        if (timestamps[i] >= timeMin && timestamps[i] <= timeMax) {
            visibleData.push_back({yData[i], timestamps[i]});
        }
    }
    
    if (visibleData.empty()) {
        qDebug() << "No data points within current time range for default scatterplot";
        return;
    }
    
    // Draw scatterplot points
    for (const auto& dataPoint : visibleData) {
        QPointF screenPoint = mapDataToScreen(dataPoint.first, dataPoint.second);
        
        // Create a circle for the scatterplot point
        QGraphicsEllipseItem* point = new QGraphicsEllipseItem();
        point->setRect(screenPoint.x() - pointSize/2, screenPoint.y() - pointSize/2, pointSize, pointSize);
        point->setPen(QPen(outlineColor, 1));
        point->setBrush(QBrush(pointColor));
        point->setZValue(120); // Draw above data lines but below markers
        
        graphicsScene->addItem(point);
    }
    
    qDebug() << "Default scatterplot drawn with" << visibleData.size() << "points";
}
