#include "btwgraph.h"
#include <QDebug>

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
{
    qDebug() << "BTWGraph constructor called";
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

    graphicsScene->clear();
    setupDrawingArea();

    if (gridEnabled) {
        drawGrid();
    }

    if (dataSource && !dataSource->isEmpty()) {
        updateDataRanges();
        drawScatterplot(Qt::red, 3.0, Qt::white); // Default scatterplot for BTW
    }
}

/**
 * @brief Handle mouse click events specific to BTW graph
 * 
 * @param scenePos Scene position of the click
 */
void BTWGraph::onMouseClick(const QPointF& scenePos)
{
    qDebug() << "BTWGraph mouse clicked at scene position:" << scenePos;
    // Call parent implementation
    WaterfallGraph::onMouseClick(scenePos);
}

/**
 * @brief Handle mouse drag events specific to BTW graph
 * 
 * @param scenePos Scene position of the drag
 */
void BTWGraph::onMouseDrag(const QPointF& scenePos)
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
    drawScatterplot(Qt::red, 4.0, Qt::white);
    
    qDebug() << "BTW scatterplot drawn";
}
