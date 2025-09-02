#include "tacticalsolutionview.h"
#include "ui_tacticalsolutionview.h"

const qreal SPEED_NORMALIZATION_FACTOR = 1;
const qreal ZOOMBOX_EXPANSION_FACTOR = 1.2;

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

/**
 * @brief Destroy the Tactical Solution View:: Tactical Solution View object
 * 
 */
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
    // DrawUtils::drawDefaultTestPattern(scene);

    drawVectors();

    qDebug() << "Draw completed - Scene rect:" << scene->sceneRect();
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

    // Our ship
    qreal ownShipSpeed = 30;
    qreal ownShipBearing = 90; //  Nautical degrees

    qreal sensorBearing = 250;

    // selected track
    qreal selectedTrackSpeed = 30;
    qreal selectedTrackRange = 50;
    qreal selectedTrackBearing = 200;

    // adopted track
    qreal adoptedTrackSpeed = 30;
    qreal adoptedTrackRange = 100;
    qreal adoptedTrackBearing = 300;

    // Create store for all the Vector Points
    VectorPointPairs pointStore;

    // Draw the ownship vector
    drawOwnShipVector(ownShipSpeed, ownShipBearing);

    // Draw the selected track vector
    drawSelectedTrackVector(sensorBearing, selectedTrackRange, selectedTrackBearing, selectedTrackSpeed);

    // Adopted track vector
    drawAdoptedTrackVector(sensorBearing, adoptedTrackRange, adoptedTrackBearing, adoptedTrackSpeed);

    // Get guidebox
    QRectF guidebox = getGuideBox(
        ownShipSpeed,
        ownShipBearing,
        sensorBearing,
        adoptedTrackRange,
        adoptedTrackSpeed,
        adoptedTrackBearing,
        selectedTrackRange,
        selectedTrackSpeed,
        selectedTrackBearing,
        &pointStore
    );



    QRectF zoomBox = getZoomBoxFromGuideBox(guidebox);

    qDebug() << "Guide Box: width: " << guidebox.width() << ", height: " << guidebox.height();
    qDebug() << "Zoom Box: width:" << zoomBox.width() << ", height: " << zoomBox.height();

    DrawUtils::addTestPattern(scene, guidebox);
    DrawUtils::addTestPattern(scene, zoomBox);

    // Compute the transformation to adjust the zooming and panning
    QTransform zoomtransform = DrawUtils::computeTransformationMatrix(zoomBox, scene->sceneRect());
    qDebug() << "Scene Box: width:" << scene->sceneRect().width() << ", height: " << scene->sceneRect().height();

    // Draw test line that goes through a point for a given angle
    // Get the largest distance
    auto largestRange = std::max(adoptedTrackRange, selectedTrackRange);
    QPointF ownShipPosition = DrawUtils::bearingToCartesian(0,0, zoomBox);

    // Use this to draw the bearings line and the bisection ref line
    auto p1 = DrawUtils::calculateEndpoint(ownShipPosition, largestRange*5, sensorBearing);
    auto p2 = DrawUtils::calculateEndpoint(ownShipPosition, largestRange*5, DrawUtils::flipBearing(sensorBearing));


    // Draw a refernce Line
    DrawUtils::addTestLine(scene, QLineF(p1, p2));
    // Draw the Bearing Line

    // Apply the transform
    // DrawUtils::transformAllSceneItems(scene, zoomtransform);

    
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
    DrawUtils::drawCourseVector(scene, ownShipPosition, ownShipSpeed / SPEED_NORMALIZATION_FACTOR, ownShipBearing, Qt::cyan);
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
    DrawUtils::drawCourseVector(scene, selectedTrackPosition, selectedTrackSpeed / SPEED_NORMALIZATION_FACTOR, selectedTrackBearing, Qt::yellow);
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
    DrawUtils::drawCourseVector(scene, adoptedTrackPosition, adoptedTrackSpeed / SPEED_NORMALIZATION_FACTOR, adoptedTrackBearing, Qt::red);
}

/**
 * @brief Calculate the Guide Box
 * 
 * @param ownShipSpeed 
 * @param ownShipBearing 
 * @param sensorBearing 
 * @param adoptedTrackRange 
 * @param adoptedTrackSpeed 
 * @param adoptedTrackBearing 
 * @param selectedTrackRange 
 * @param selectedTrackSpeed 
 * @param selectedTrackBearing 
 * @return QRectF 
 */
QRectF TacticalSolutionView::getGuideBox(
    qreal ownShipSpeed,
    qreal ownShipBearing,
    qreal sensorBearing,
    qreal adoptedTrackRange,
    qreal adoptedTrackSpeed,
    qreal adoptedTrackBearing,
    qreal selectedTrackRange,
    qreal selectedTrackSpeed,
    qreal selectedTrackBearing,
    VectorPointPairs* pointStore
)
{
    std::vector<QPointF> guideBoxPoints;

    // Own Ship Vector
    QPointF ownShipBearingPosition = QPointF(0, 0);

    QPointF ownShipPosition = DrawUtils::bearingToCartesian(
        0,
        0,
        this->scene->sceneRect());

    auto endpoint = DrawUtils::calculateEndpoint(ownShipPosition, ownShipSpeed, ownShipBearing);

    // Add to the guidebox list
    guideBoxPoints.push_back(ownShipPosition);
    guideBoxPoints.push_back(endpoint);

    // Store the points
    pointStore->ownShipPoints = qMakePair(ownShipBearingPosition, endpoint);

    // Selected Track Vector
    QPointF selectedTrackPosition = DrawUtils::bearingToCartesian(
        selectedTrackRange,
        sensorBearing,
        this->scene->sceneRect());

    endpoint = DrawUtils::calculateEndpoint(selectedTrackPosition, selectedTrackSpeed, selectedTrackBearing);

    // Add to the guidebox list
    guideBoxPoints.push_back(selectedTrackPosition);
    guideBoxPoints.push_back(endpoint);
    
    // Store the points
    pointStore->ownShipPoints = qMakePair(selectedTrackPosition, endpoint);

    // Adopted Track Vector
    QPointF adoptedTrackPosition = DrawUtils::bearingToCartesian(
        adoptedTrackRange,
        sensorBearing,
        this->scene->sceneRect());

    endpoint = DrawUtils::calculateEndpoint(adoptedTrackPosition, adoptedTrackSpeed, adoptedTrackBearing);

    // Add to the guidebox list
    guideBoxPoints.push_back(adoptedTrackPosition);
    guideBoxPoints.push_back(endpoint);
    
    // Store the points
    pointStore->ownShipPoints = qMakePair(adoptedTrackPosition, endpoint);

    // Loop throught the guidebox points and find the min/max x,y co-ordinates amongt
    qreal xmin = 0;
    qreal xmax = 0;
    qreal ymin = 0;
    qreal ymax = 0;

    QPointF firstelement = guideBoxPoints.front();

    xmin = firstelement.x();
    xmax = firstelement.x();

    ymin = firstelement.y();
    ymax = firstelement.y();

    // Loop through all the points to find the extent of the guidebox
    for (const QPointF &element : guideBoxPoints)
    {
        if (element.x() > xmax)
        {
            xmax = element.x();
        }

        if (element.x() < xmin)
        {
            xmin = element.x();
        }

        if (element.y() > ymax)
        {
            ymax = element.y();
        }

        if (element.y() < ymin)
        {
            ymin = element.y();
        }
    }

    // Here's the box needed
    QRectF guidebox = QRectF(
        xmin,
        ymin,
        xmax - xmin,
        ymax - ymin);

    return guidebox;
}

/**
 * @brief Returns the zoom box from the guidebox
 * 
 * @param guidebox 
 * @return QRectF 
 */
QRectF TacticalSolutionView::getZoomBoxFromGuideBox(const QRectF guidebox)
{
    // Look for the biggest side and save it
    qreal largestSide;

    if (guidebox.width() > guidebox.height())
    {
        largestSide = guidebox.width();
    } else {
        largestSide = guidebox.height();
    }

    // Now expand the dimension
    largestSide = ZOOMBOX_EXPANSION_FACTOR * largestSide;

    // Now create a new rectagle with the same center as the previous

    qreal deltax = (largestSide - guidebox.width())/2;
    qreal deltay = (largestSide - guidebox.height())/2;

    QRectF zoomBox = QRectF(guidebox);

    zoomBox.adjust(
        -deltax,
        -deltay,
        deltax,
        deltay
    );

    return zoomBox;
}
