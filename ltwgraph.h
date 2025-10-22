#ifndef LTWGRAPH_H
#define LTWGRAPH_H

#include "waterfallgraph.h"
#include <QGraphicsLineItem>

/**
 * @brief LTW Graph component that inherits from waterfallgraph
 *
 * This component creates scatterplots by default and can be extended
 * for specific LTW (Latency Time Waterfall) functionality.
 * Features a cyan crosshair that tracks the cursor.
 */
class LTWGraph : public WaterfallGraph
{
    Q_OBJECT

public:
    explicit LTWGraph(QWidget *parent = nullptr, bool enableGrid = false, int gridDivisions = 10, TimeInterval timeInterval = TimeInterval::FifteenMinutes);
    ~LTWGraph();

protected:
    // Override the draw method to create scatterplots by default
    void draw() override;

    // Override mouse event handlers if needed
    void onMouseClick(const QPointF &scenePos) override;
    void onMouseDrag(const QPointF &scenePos) override;
    
    // Override mouse move to track cursor for crosshair
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    // LTW-specific properties and methods can be added here
    void drawLTWScatterplot();
    void drawCustomMarkers(const QString &seriesLabel, const QColor &markerColor);
    
    // Crosshair functionality
    void setupCrosshair();
    void updateCrosshair(const QPointF &mousePos);
    void showCrosshair();
    void hideCrosshair();
    
    // Crosshair graphics items
    QGraphicsLineItem *crosshairHorizontal;
    QGraphicsLineItem *crosshairVertical;
    bool crosshairVisible;
};

#endif // LTWGRAPH_H
