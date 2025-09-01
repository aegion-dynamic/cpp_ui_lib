#include "tacticalsolutionview.h"
#include "ui_tacticalsolutionview.h"
#include "drawutils.h"


/**
 * @brief Construct a new Tactical Solution View:: Tactical Solution View object
 *
 * @param parent
 */
TacticalSolutionView::TacticalSolutionView(QWidget *parent)
    : QWidget(parent), ui(new Ui::TacticalSolutionView)
{
    ui->setupUi(this);

    // Set black background
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);

    // Set minimum size
    setMinimumSize(226, 240);

    // Initialize scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, width(), height());

    // Make sure widget expands
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Enable mouse tracking
    // setMouseTracking(true);
}

TacticalSolutionView::~TacticalSolutionView()
{
    delete ui;
}

/**
 * @brief Draw the entire graph.
 *
 */
void TacticalSolutionView::draw()
{
    if (!scene)
        return;

    // Clear existing items
    scene->clear();

    // Update scene rect to match widget size
    scene->setSceneRect(0, 0, width(), height());

    // Draw in layers from back to front
    drawBackground();
    drawTestPattern();

    drawVectors(QPointF(50, 50), QPointF(150, 100), QPointF(100, 200));

    qDebug() << "Draw completed - Scene rect:" << scene->sceneRect();
}

/**
 * @brief Draw a test pattern for debugging purposes.
 *
 */
void TacticalSolutionView::drawTestPattern()
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
 * @brief Draw the background for the graph.
 *
 */
void TacticalSolutionView::drawBackground()
{
    if (!scene)
        return;

    // Draw the background - currently empty as we use widget background
}

/**
 * @brief Handle paint events for the graph.
 *
 * @param event
 */
void TacticalSolutionView::paintEvent(QPaintEvent *event)
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

void TacticalSolutionView::drawVectors(QPointF ownShipPosition, QPointF selectedTrackPosition, QPointF adoptedPosition)
{
    
    if (!scene)
    {
        return;
    }
    int magnitude = 100;
    double bearing = 45; // degrees

    // Draw a figure (circle) at each position
    drawCourseVector(scene, ownShipPosition, magnitude, bearing, Qt::red);
    drawCourseVector(scene, selectedTrackPosition, magnitude, bearing, Qt::green);
    drawCourseVector(scene, adoptedPosition, magnitude, bearing, Qt::blue);
}



void TacticalSolutionView::drawCourseVector(QGraphicsScene* scene , QPointF startPoint, double magnitude, double bearing, const QColor& color)
{
    QPen pen(color);
    QBrush brush(color);
    int radius = 5;
    scene->addEllipse(startPoint.x() - radius, startPoint.y() - radius, radius * 2, radius * 2, pen, brush);

    // Calulate endpoint

    auto endpoint = DrawUtils::calculateEndpoint(startPoint, magnitude, bearing);

    pen.setWidth(2);
    scene->addLine(QLineF(startPoint, endpoint), pen);
}
