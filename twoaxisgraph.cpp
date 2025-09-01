#include "twoaxisgraph.h"
#include "ui_twoaxisgraph.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsSimpleTextItem>

const int MAX_EVENT_COUNT = 100;

/**
 * @brief Construct a new twoaxisgraph::twoaxisgraph object
 *
 * @param parent
 */
twoaxisgraph::twoaxisgraph(QWidget *parent)
    : QWidget(parent), ui(new Ui::twoaxisgraph), scene(nullptr)
{
    ui->setupUi(this);

    // Set black background
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);

    // Set minimum size
    setMinimumSize(400, 300);

    // Initialize scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, width(), height());

    // Make sure widget expands
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Enable mouse tracking
    setMouseTracking(true);

    // Initial draw will happen in paintEvent
}

/**
 * @brief Destroy the twoaxisgraph::twoaxisgraph object
 *
 */
twoaxisgraph::~twoaxisgraph()
{
    delete ui;
}

/**
 * @brief Handle paint events for the graph.
 *
 * @param event
 */
void twoaxisgraph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (scene)
    {
        // Draw all elements
        draw();

        // Render the scene to the widget
        scene->render(&painter, rect(), scene->sceneRect());
    }

    qDebug() << "Paint event - Widget size:" << width() << "x" << height();
}

/**
 * @brief Handle resize events for the graph.
 *
 * @param event
 */
void twoaxisgraph::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (scene)
    {
        // Redraw will happen in paintEvent
        update();

        qDebug() << "Resize event - New size:" << size();
    }
}

/**
 * @brief Draw the entire graph.
 *
 */
void twoaxisgraph::draw()
{
    if (!scene)
        return;

    // Clear existing items
    scene->clear();

    // Update scene rect to match widget size
    scene->setSceneRect(0, 0, width(), height());

    // Draw in layers from back to front
    drawBackground();
    drawGraphArea();
    drawAxes();
    drawInfoArea();
    // drawTestPattern();
    drawCursor();

    qDebug() << "Draw completed - Scene rect:" << scene->sceneRect();
}

/**
 * @brief Draw the background for the graph.
 *
 */
void twoaxisgraph::drawBackground()
{
    if (!scene)
        return;

    // Draw the background - currently empty as we use widget background
}

/**
 * @brief Draw the graph area for the plot.
 *
 */
void twoaxisgraph::drawGraphArea()
{
    if (!scene)
        return;

    QPen areaPen(Qt::gray, 1);
    areaPen.setStyle(Qt::DashLine);

    // Draw the graph area rectangle as a light boundary
    scene->addRect(getGraphDrawArea(), areaPen);
}

/**
 * @brief Set the data for the graph.
 *
 * @param x
 * @param y1
 * @param y2
 * @return true
 * @return false
 */
bool twoaxisgraph::setData(const std::vector<double> &x,
                           const std::vector<double> &y1,
                           const std::vector<double> &y2)
{
    if (data.setData(x, y1, y2))
    {
        update(); // Trigger redraw
        return true;
    }
    return false;
}

/**
 * @brief Draw the axes for the graph.
 *
 */
void twoaxisgraph::drawAxes()
{
    if (!scene)
        return;

    QRectF graphArea = getGraphDrawArea();

    // Set up pens for different axes
    QPen leftAxisPen(Qt::yellow, 1);
    QPen rightAxisPen(Qt::green, 1);
    QPen bottomAxisPen(Qt::white, 1);

    // All pens use round cap for clean endings
    leftAxisPen.setCapStyle(Qt::RoundCap);
    rightAxisPen.setCapStyle(Qt::RoundCap);
    bottomAxisPen.setCapStyle(Qt::RoundCap);

    // Draw left Y axis (Yellow)
    scene->addLine(
        graphArea.left(), graphArea.top(),
        graphArea.left(), graphArea.bottom(),
        leftAxisPen);

    // Draw right Y axis (Green)
    scene->addLine(
        graphArea.right(), graphArea.top(),
        graphArea.right(), graphArea.bottom(),
        rightAxisPen);

    // Draw bottom X axis (White)
    scene->addLine(
        graphArea.left(), graphArea.bottom(),
        graphArea.right(), graphArea.bottom(),
        bottomAxisPen);

    // Add tick marks
    const int tickLength = 5; // Length of tick marks in pixels
    const int numTicks = 10;  // Number of ticks per axis

    // Draw Y-axis ticks (left and right)
    for (int i = 0; i <= numTicks; ++i)
    {
        qreal y = graphArea.top() + (i * graphArea.height() / numTicks);

        // Left ticks (Yellow)
        scene->addLine(
            graphArea.left() - tickLength, y,
            graphArea.left(), y,
            leftAxisPen);

        // Right ticks (Green)
        scene->addLine(
            graphArea.right(), y,
            graphArea.right() + tickLength, y,
            rightAxisPen);
    }

    // Draw X-axis ticks (White)
    for (int i = 0; i <= numTicks; ++i)
    {
        qreal x = graphArea.left() + (i * graphArea.width() / numTicks);
        scene->addLine(
            x, graphArea.bottom(),
            x, graphArea.bottom() + tickLength,
            bottomAxisPen);
    }
}

/**
 * @brief Draw a test pattern for debugging purposes.
 *
 */
void twoaxisgraph::drawTestPattern()
{
    if (!scene)
        return;

    QPen testPen(Qt::white, 1);
    testPen.setStyle(Qt::DashLine);

    // Draw test rectangle showing full bounds
    scene->addRect(scene->sceneRect(), testPen);

    // Draw diagonal line to show extent
    scene->addLine(0, 0, width(), height(), testPen);
}

/**
 * @brief Draw the data lines for the graph.
 *
 */
void twoaxisgraph::drawData()
{
    if (!scene)
        return;

    const auto &x = data.getXData();
    const auto &y1 = data.getY1Data();
    const auto &y2 = data.getY2Data();

    if (x.empty())
        return;

    QRectF graphArea = getGraphDrawArea();

    // Create pens for each dataset
    QPen y1Pen(Qt::yellow, 1);
    QPen y2Pen(Qt::green, 1);

    // Draw Y1 data (left axis)
    for (size_t i = 1; i < x.size(); ++i)
    {
        // Convert data points to screen coordinates
        qreal x1 = graphArea.left() + (x[i - 1] - data.getXMin()) / (data.getXMax() - data.getXMin()) * graphArea.width();
        qreal y1_1 = graphArea.bottom() - (y1[i - 1] - data.getY1Min()) / (data.getY1Max() - data.getY1Min()) * graphArea.height();
        qreal x2 = graphArea.left() + (x[i] - data.getXMin()) / (data.getXMax() - data.getXMin()) * graphArea.width();
        qreal y1_2 = graphArea.bottom() - (y1[i] - data.getY1Min()) / (data.getY1Max() - data.getY1Min()) * graphArea.height();

        scene->addLine(x1, y1_1, x2, y1_2, y1Pen);
    }

    // Draw Y2 data (right axis)
    for (size_t i = 1; i < x.size(); ++i)
    {
        // Convert data points to screen coordinates
        qreal x1 = graphArea.left() + (x[i - 1] - data.getXMin()) / (data.getXMax() - data.getXMin()) * graphArea.width();
        qreal y2_1 = graphArea.bottom() - (y2[i - 1] - data.getY2Min()) / (data.getY2Max() - data.getY2Min()) * graphArea.height();
        qreal x2 = graphArea.left() + (x[i] - data.getXMin()) / (data.getXMax() - data.getXMin()) * graphArea.width();
        qreal y2_2 = graphArea.bottom() - (y2[i] - data.getY2Min()) / (data.getY2Max() - data.getY2Min()) * graphArea.height();

        scene->addLine(x1, y2_1, x2, y2_2, y2Pen);
    }
}

/**
 * @brief Draw the axis labels for the graph.
 *
 */
void twoaxisgraph::drawAxesLabels()
{
    if (!scene)
        return;

    // Create a font for the text
    QFont infoFont("Arial", 8);

    // Create a text item
    QGraphicsSimpleTextItem *bottomAxesLabelItem = new QGraphicsSimpleTextItem("Simple Text");
    bottomAxesLabelItem->setBrush(Qt::white);
    bottomAxesLabelItem->setFont(infoFont);

    // Calculate the position for bottom center
    QRectF bounds = bottomAxesLabelItem->boundingRect();
    qreal x = (scene->width() - bounds.width()) / 2; // Center horizontally
    qreal y = scene->height() - bounds.height() - 5; // 5 pixels from bottom

    bottomAxesLabelItem->setPos(x, y);
    scene->addItem(bottomAxesLabelItem);

    // Left axis label
    QGraphicsSimpleTextItem *leftAxesLabelItem = new QGraphicsSimpleTextItem("Left Axis");
    leftAxesLabelItem->setBrush(Qt::white);
    leftAxesLabelItem->setFont(infoFont);

    QRectF leftBounds = leftAxesLabelItem->boundingRect();
    qreal leftX = 5;                                           // 5 pixels from left edge
    qreal leftY = (scene->height() - leftBounds.height()) / 2; // Center vertically

    leftAxesLabelItem->setPos(leftX, leftY);
    scene->addItem(leftAxesLabelItem);

    // Right axis label
    QGraphicsSimpleTextItem *rightAxesLabelItem = new QGraphicsSimpleTextItem("Right Axis");
    rightAxesLabelItem->setBrush(Qt::white);
    rightAxesLabelItem->setFont(infoFont);

    QRectF rightBounds = rightAxesLabelItem->boundingRect();
    qreal rightX = scene->width() - rightBounds.width() - 5;     // 5 pixels from right edge
    qreal rightY = (scene->height() - rightBounds.height()) / 2; // Center vertically

    rightAxesLabelItem->setPos(rightX, rightY);
    scene->addItem(rightAxesLabelItem);
}

/**
 * @brief Draw the information area for the graph.
 *
 */
void twoaxisgraph::drawInfoArea()
{
    if (!scene)
        return;

    // Draw axes labels
    drawAxesLabels();
}

/**
 * @brief Check if the event should be processed based on the drop percentage.
 *
 * @return true if the event should be processed, false otherwise.
 */
bool twoaxisgraph::shouldProcessEvent()
{
    eventCount++;

    // Process event if it falls outside the drop percentage
    bool shouldProcess = (eventCount % 100) > dropPercentage;

    // Reset counter to prevent overflow
    if (eventCount > MAX_EVENT_COUNT)
    {
        eventCount = 0;
    }

    return shouldProcess;
}

/**
 * @brief Handle mouse move events for the graph.
 *
 * @param event
 */
void twoaxisgraph::mouseMoveEvent(QMouseEvent *event)
{
    if (!scene) return;

    // Apply debounce
    // if (!shouldProcessEvent()) {
    //     return;
    // }

    currentMousePos = event->pos();
    
    qDebug() << "Mouse Position [" << eventCount << "] -" 
             << "Widget:" << currentMousePos;
            //  << "(Processed" << (100 - dropPercentage) << "% of events)";
    
    // Trigger redraw to update cursor
    update();
}

/**
 * @brief Get the scene coordinates from the widget position.
 *
 * @param widgetPos
 * @return qreal
 */
qreal twoaxisgraph::getSceneCoordinates(const QPoint &widgetPos) const
{
    // Get the graph area
    QRectF graphArea = getGraphDrawArea();

    // Convert widget coordinates to graph coordinates
    qreal xRatio = (widgetPos.x() - graphArea.left()) / graphArea.width();

    // Map to actual data ranges
    qreal x = data.getXMin() + (data.getXMax() - data.getXMin()) * xRatio;

    return x;
}

/**
 * @brief Create a styled axis label.
 *
 * @param text
 * @param textColor
 * @param borderColor
 * @param backgroundColor
 * @return QGraphicsTextItem*
 */
QGraphicsTextItem *twoaxisgraph::createAxisLabel(const QString &text,
                                                 const QColor &textColor,
                                                 const QColor &borderColor,
                                                 const QColor &backgroundColor)
{
    QGraphicsTextItem *textItem = new QGraphicsTextItem();
    textItem->setPlainText(text);
    textItem->setDefaultTextColor(textColor);
    textItem->setFont(QFont("Arial", 8));

    // Create background rectangle
    QRectF rect = textItem->boundingRect();
    rect.adjust(-4, -2, 4, 2); // Add padding

    // QGraphicsRectItem* background = new QGraphicsRectItem(rect, textItem);
    // background->setBrush(backgroundColor);
    // background->setPen(QPen(borderColor));
    // background->setZValue(-1);  // Place behind text

    return textItem;
}

/**
 * @brief Draw the cursor for the graph.
 *
 */
void twoaxisgraph::drawCursor()
{
    if (!scene)
        return;

    QRectF graphArea = getGraphDrawArea();

    // Only draw cursor if it's within the graph area
    if (!graphArea.contains(currentMousePos))
    {
        return;
    }

    // Create a cross hair cursor
    QPen cursorPen(Qt::white, 1);     // White color
    cursorPen.setStyle(Qt::DashLine); // Dashed line for better visibility

    // Draw vertical line from top to bottom of graph area
    scene->addLine(currentMousePos.x(), graphArea.top(),
                   currentMousePos.x(), graphArea.bottom(),
                   cursorPen);

    // Draw horizontal line from left to right of graph area
    scene->addLine(graphArea.left(), currentMousePos.y(),
                   graphArea.right(), currentMousePos.y(),
                   cursorPen);

    // Get normalized coordinates
    qreal xcoord = getSceneCoordinates(currentMousePos);

    // Get values at cursor position
    qreal y1Value = data.getY1AtX(xcoord);
    qreal y2Value = data.getY2AtX(xcoord);
    qreal xValue = xcoord;

    // Create coordinate labels
    QString leftText = QString::number(y1Value, 'f', 1);
    QString rightText = QString::number(y2Value, 'f', 1);
    QString bottomText = QString::number(xValue, 'f', 1);

    // Create and position left axis label (yellow)
    QGraphicsTextItem *leftLabel = createAxisLabel(leftText, Qt::yellow, Qt::darkYellow, Qt::black);
    leftLabel->setPos(graphArea.left() - leftLabel->boundingRect().width() - 8,
                      currentMousePos.y() - leftLabel->boundingRect().height() / 2);

    // Create and position right axis label (green)
    QGraphicsTextItem *rightLabel = createAxisLabel(rightText, Qt::green, Qt::darkGreen, Qt::black);
    rightLabel->setPos(graphArea.right() + 8,
                       currentMousePos.y() - rightLabel->boundingRect().height() / 2);

    // Create and position bottom axis label (white)
    QGraphicsTextItem *bottomLabel = createAxisLabel(bottomText, Qt::white, Qt::gray, Qt::black);
    bottomLabel->setPos(currentMousePos.x() - bottomLabel->boundingRect().width() / 2,
                        graphArea.bottom() + 8);

    // Add labels to scene
    scene->addItem(leftLabel);
    scene->addItem(rightLabel);
    scene->addItem(bottomLabel);
}

/**
 * @brief Get the drawing area for the graph.
 *
 * @return QRectF
 */
QRectF twoaxisgraph::getGraphDrawArea() const
{
    //
    if (!scene->sceneRect().isValid())
    {
        return QRectF();
    }

    // This is the area where we reserve to draw the graph
    QRectF sceneRect = scene->sceneRect();

    if (!sceneRect.isValid())
    {
        return QRectF();
    }

    // This is the area where we reserve to draw the graph
    QRectF graphArea = sceneRect;

    // Reserve 20% of each side for labels/info
    qreal hMargin = graphArea.width() * 0.10;
    qreal vMargin = graphArea.height() * 0.15;

    graphArea.adjust(hMargin, vMargin, -hMargin, -vMargin);
    return graphArea;
}
