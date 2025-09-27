#include "bdwgraph.h"
#include <QDebug>

/**
 * @brief Construct a new BDWGraph::BDWGraph object
 * 
 * @param parent Parent widget
 * @param enableGrid Whether to enable grid display
 * @param gridDivisions Number of grid divisions
 * @param timeInterval Time interval for the waterfall display
 */
BDWGraph::BDWGraph(QWidget *parent, bool enableGrid, int gridDivisions, TimeInterval timeInterval)
    : waterfallgraph(parent, enableGrid, gridDivisions, timeInterval)
{
    qDebug() << "BDWGraph constructor called";
}

/**
 * @brief Destroy the BDWGraph::BDWGraph object
 * 
 */
BDWGraph::~BDWGraph()
{
    qDebug() << "BDWGraph destructor called";
}

/**
 * @brief Override draw method to create scatterplots by default
 * 
 */
void BDWGraph::draw()
{
    // Call parent draw method first
    waterfallgraph::draw();
    
    // Add BDW-specific scatterplot drawing
    drawBDWScatterplot();
}

/**
 * @brief Handle mouse click events specific to BDW graph
 * 
 * @param scenePos Scene position of the click
 */
void BDWGraph::onMouseClick(const QPointF& scenePos)
{
    qDebug() << "BDWGraph mouse clicked at scene position:" << scenePos;
    // Call parent implementation
    waterfallgraph::onMouseClick(scenePos);
}

/**
 * @brief Handle mouse drag events specific to BDW graph
 * 
 * @param scenePos Scene position of the drag
 */
void BDWGraph::onMouseDrag(const QPointF& scenePos)
{
    qDebug() << "BDWGraph mouse dragged to scene position:" << scenePos;
    // Call parent implementation
    waterfallgraph::onMouseDrag(scenePos);
}

/**
 * @brief Draw BDW-specific scatterplot
 * 
 */
void BDWGraph::drawBDWScatterplot()
{
    // By default, create a scatterplot using the parent's scatterplot functionality
    drawScatterplot(Qt::magenta, 4.0, Qt::white);
    
    qDebug() << "BDW scatterplot drawn";
}
