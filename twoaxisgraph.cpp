#include "twoaxisgraph.h"
#include "ui_twoaxisgraph.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsSimpleTextItem>

const int MAX_EVENT_COUNT = 100;

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

twoaxisgraph::~twoaxisgraph()
{
    delete ui;
}

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

void twoaxisgraph::drawBackground()
{
    if (!scene)
        return;

    // Draw the background - currently empty as we use widget background
}

void twoaxisgraph::drawGraphArea()
{
    if (!scene)
        return;

    QPen areaPen(Qt::gray, 1);
    areaPen.setStyle(Qt::DashLine);

    // Draw the graph area rectangle as a light boundary
    scene->addRect(getGraphDrawArea(), areaPen);
}

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
        leftAxisPen
    );

    // Draw right Y axis (Green)
    scene->addLine(
        graphArea.right(), graphArea.top(),
        graphArea.right(), graphArea.bottom(),
        rightAxisPen
    );

    // Draw bottom X axis (White)
    scene->addLine(
        graphArea.left(), graphArea.bottom(),
        graphArea.right(), graphArea.bottom(),
        bottomAxisPen
    );

    // Add tick marks
    const int tickLength = 5;  // Length of tick marks in pixels
    const int numTicks = 10;   // Number of ticks per axis

    // Draw Y-axis ticks (left and right)
    for (int i = 0; i <= numTicks; ++i) {
        qreal y = graphArea.top() + (i * graphArea.height() / numTicks);
        
        // Left ticks (Yellow)
        scene->addLine(
            graphArea.left() - tickLength, y,
            graphArea.left(), y,
            leftAxisPen
        );
        
        // Right ticks (Green)
        scene->addLine(
            graphArea.right(), y,
            graphArea.right() + tickLength, y,
            rightAxisPen
        );
    }

    // Draw X-axis ticks (White)
    for (int i = 0; i <= numTicks; ++i) {
        qreal x = graphArea.left() + (i * graphArea.width() / numTicks);
        scene->addLine(
            x, graphArea.bottom(),
            x, graphArea.bottom() + tickLength,
            bottomAxisPen
        );
    }
}

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

void twoaxisgraph::drawAxesLabels()
{
    if (!scene)
        return;

    // Create a font for the text
    // Create a font for the text
    QFont infoFont("Arial", 5);

    // Create a text item
    QGraphicsSimpleTextItem* bottomAxesLabelItem = new QGraphicsSimpleTextItem("Simple Text");
    bottomAxesLabelItem->setBrush(Qt::white);
    bottomAxesLabelItem->setFont(infoFont);

    // Calculate the position for bottom center
    QRectF bounds = bottomAxesLabelItem->boundingRect();
    qreal x = (scene->width() - bounds.width()) / 2;  // Center horizontally
    qreal y = scene->height() - bounds.height() - 5; // 5 pixels from bottom

    bottomAxesLabelItem->setPos(x, y);
    scene->addItem(bottomAxesLabelItem);

    // Left axis label
    QGraphicsSimpleTextItem* leftAxesLabelItem = new QGraphicsSimpleTextItem("Left Axis");
    leftAxesLabelItem->setBrush(Qt::white);
    leftAxesLabelItem->setFont(infoFont);

    QRectF leftBounds = leftAxesLabelItem->boundingRect();
    qreal leftX = 5; // 5 pixels from left edge
    qreal leftY = (scene->height() - leftBounds.height()) / 2; // Center vertically

    leftAxesLabelItem->setPos(leftX, leftY);
    scene->addItem(leftAxesLabelItem);

    // Right axis label
    QGraphicsSimpleTextItem* rightAxesLabelItem = new QGraphicsSimpleTextItem("Right Axis");
    rightAxesLabelItem->setBrush(Qt::white);
    rightAxesLabelItem->setFont(infoFont);

    QRectF rightBounds = rightAxesLabelItem->boundingRect();
    qreal rightX = scene->width() - rightBounds.width() - 5; // 5 pixels from right edge
    qreal rightY = (scene->height() - rightBounds.height()) / 2; // Center vertically

    rightAxesLabelItem->setPos(rightX, rightY);
    scene->addItem(rightAxesLabelItem);
}

void twoaxisgraph::drawInfoArea()
{
    if (!scene)
        return;

    // Draw axes labels
    drawAxesLabels();

}

bool twoaxisgraph::shouldProcessEvent()
{
    eventCount++;
    
    // Process event if it falls outside the drop percentage
    bool shouldProcess = (eventCount % 100) > dropPercentage;
    
    // Reset counter to prevent overflow
    if (eventCount > MAX_EVENT_COUNT) {
        eventCount = 0;
    }
    
    return shouldProcess;
}

void twoaxisgraph::mouseMoveEvent(QMouseEvent *event)
{
    if (!scene) return;

    // Apply debounce
    // if (!shouldProcessEvent()) {
    //     return;
    // }

    currentMousePos = event->pos();
    QPointF scenePos = getSceneCoordinates(currentMousePos);
    
    qDebug() << "Mouse Position [" << eventCount << "] -" 
             << "Widget:" << currentMousePos
             << "Graph:" << scenePos;
            //  << "(Processed" << (100 - dropPercentage) << "% of events)";
    
    // Trigger redraw to update cursor
    update();
}

QPointF twoaxisgraph::getSceneCoordinates(const QPoint& widgetPos) const
{
    // Get the graph area
    QRectF graphArea = getGraphDrawArea();
    
    // Convert widget coordinates to graph coordinates
    qreal x = (widgetPos.x() - graphArea.left()) / graphArea.width();
    qreal y = 1.0 - (widgetPos.y() - graphArea.top()) / graphArea.height(); // Invert Y
    
    return QPointF(x * 100, y * 100); // Scale to 0-100 range
}

void twoaxisgraph::drawCursor()
{
    if (!scene) return;

    QRectF graphArea = getGraphDrawArea();
    
    // Only draw cursor if it's within the graph area
    if (!graphArea.contains(currentMousePos)) {
        return;
    }

    // Create a cross hair cursor
    QPen cursorPen(Qt::red, 1);  // Red color for visibility
    cursorPen.setStyle(Qt::SolidLine);
    
    const int crossSize = 10;  // Size of the cross in pixels
    
    // Draw vertical line
    scene->addLine(currentMousePos.x(), currentMousePos.y() - crossSize,
                  currentMousePos.x(), currentMousePos.y() + crossSize,
                  cursorPen);
    
    // Draw horizontal line
    scene->addLine(currentMousePos.x() - crossSize, currentMousePos.y(),
                  currentMousePos.x() + crossSize, currentMousePos.y(),
                  cursorPen);
}

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
