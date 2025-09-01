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

    drawVectors();

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

/// @brief Draws the various vectors
/// @param ownShipPosition
/// @param selectedTrackPosition
/// @param adoptedPosition
void TacticalSolutionView::drawVectors()
{

    if (!scene)
    {
        return;
    }
    int magnitude = 30;
    double bearing = 45; // degrees
    qreal selectedTrackSpeed = 50;
    qreal selectedTrackBearing = 200;

    qreal sensorBearing = 250;

    qreal adoptedTrackSpeed = 100;
    qreal adoptedTrackBearing = 300;

    QPointF selectedTrackPosition = QPointF(150, 100);
    QPointF adoptedPosition = QPointF(100, 200);

    // Draw the ownship vector
    drawOwnShipVector(magnitude, bearing);

    // Draw the selected track vector
    drawSelectedTrackVector(sensorBearing, selectedTrackSpeed, selectedTrackBearing, magnitude);

    // Adopted track vector
    drawAdoptedTrackVector(sensorBearing, adoptedTrackSpeed, adoptedTrackBearing, magnitude);
}

/**
 * @brief Draws the own ship vector
 *
 * @param magnitude
 * @param bearing
 */
void TacticalSolutionView::drawOwnShipVector(qreal ownShipSpeed, qreal ownShipBearing)
{
    QPointF ownShipBearingPosition = QPointF(0, 0);
    QPointF ownShipPosition = DrawUtils::bearingToCartesian(
        0,
        0,
        this->scene->sceneRect());
    // Draw a figure (circle) at each position
    DrawUtils::drawCourseVector(scene, ownShipPosition, ownShipSpeed, ownShipBearing, Qt::cyan);
}

/**
 * @brief Draws the selected track vector
 *
 * @param sensorBearing
 * @param selectedTrackSpeed
 * @param selectedTrackBearing
 * @param magnitude
 */
void TacticalSolutionView::drawSelectedTrackVector(qreal sensorBearing, qreal selectedTrackRange, qreal selectedTrackBearing, qreal selectedTrackSpeed)
{
    QPointF selectedTrackPosition = DrawUtils::bearingToCartesian(
        selectedTrackRange,
        sensorBearing,
        this->scene->sceneRect());
    // Draw a figure (circle) at each position
    DrawUtils::drawCourseVector(scene, selectedTrackPosition, selectedTrackSpeed, selectedTrackBearing, Qt::yellow);
}

/**
 * @brief Draws the adopted track vector
 *
 * @param sensorBearing
 * @param adoptedTrackSpeed
 * @param adoptedTrackBearing
 * @param magnitude
 */
void TacticalSolutionView::drawAdoptedTrackVector(qreal sensorBearing, qreal adoptedTrackRange, qreal adoptedTrackBearing, qreal adoptedTrackSpeed)
{
    QPointF adoptedTrackPosition = DrawUtils::bearingToCartesian(
        adoptedTrackRange,
        sensorBearing,
        this->scene->sceneRect());
    // Draw a figure (circle) at each position
    DrawUtils::drawCourseVector(scene, adoptedTrackPosition, adoptedTrackSpeed, adoptedTrackBearing, Qt::red);
}

QRectF TacticalSolutionView::getGuideBox(
    qreal ownShipSpeed,
    qreal ownShipBearing,
    qreal sensorBearing,
    qreal adoptedTrackRange,
    qreal adoptedTrackSpeed,
    qreal adoptedTrackBearing,
    qreal selectedTrackRange,
    qreal selectedTrackSpeed,
    qreal selectedTrackBearing
)
{
    std::vector<QPointF> guideBoxPoints;

    // Own ship position
    QPointF ownShipBearingPosition = QPointF(0, 0);
    QPointF ownShipPosition = DrawUtils::bearingToCartesian(
        0,
        0,
        this->scene->sceneRect());

    // Add it to the guideBoxPoints
    guideBoxPoints.push_back(ownShipPosition);
}
