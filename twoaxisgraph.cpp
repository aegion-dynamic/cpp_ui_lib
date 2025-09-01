#include "twoaxisgraph.h"
#include "ui_twoaxisgraph.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsSimpleTextItem>

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
    drawInfoArea();
    drawTestPattern();

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

    QPen axisPen(Qt::white, 2);
    axisPen.setCapStyle(Qt::RoundCap);

    // Draw the graph area rectangle
    scene->addRect(getGraphDrawArea(), axisPen);
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

void twoaxisgraph::mouseMoveEvent(QMouseEvent *event)
{
    if (!scene) return;

    QPoint pos = event->pos();
    QPointF scenePos = getSceneCoordinates(pos);
    
    qDebug() << "Mouse Position -" 
             << "Widget:" << pos
             << "Graph:" << scenePos;
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
    qreal vMargin = graphArea.height() * 0.10;

    graphArea.adjust(hMargin, vMargin, -hMargin, -vMargin);
    return graphArea;
}
