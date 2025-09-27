#ifndef LTWGRAPH_H
#define LTWGRAPH_H

#include "waterfallgraph.h"

/**
 * @brief LTW Graph component that inherits from waterfallgraph
 * 
 * This component creates scatterplots by default and can be extended
 * for specific LTW (Latency Time Waterfall) functionality.
 */
class LTWGraph : public waterfallgraph
{
    Q_OBJECT

public:
    explicit LTWGraph(QWidget *parent = nullptr, bool enableGrid = false, int gridDivisions = 10, TimeInterval timeInterval = TimeInterval::FifteenMinutes);
    ~LTWGraph();

protected:
    // Override the draw method to create scatterplots by default
    void draw() override;
    
    // Override mouse event handlers if needed
    void onMouseClick(const QPointF& scenePos) override;
    void onMouseDrag(const QPointF& scenePos) override;

private:
    // LTW-specific properties and methods can be added here
    void drawLTWScatterplot();
};

#endif // LTWGRAPH_H
