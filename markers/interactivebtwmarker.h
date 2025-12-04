#ifndef INTERACTIVEBTWMARKER_H
#define INTERACTIVEBTWMARKER_H

#include "waterfallmarker.h"
#include <QDateTime>
#include <QPointF>
#include <QGraphicsScene>
#include <QList>

// Forward declaration
class InteractiveGraphicsItem;

/**
 * @brief Interactive BTW marker - creates and manages its own InteractiveGraphicsItem
 */
class InteractiveBTWMarker : public InteractiveWaterfallMarker
{
public:
    InteractiveBTWMarker(const QDateTime &timestamp, qreal value, const QPointF &position, QGraphicsScene *overlayScene);
    virtual ~InteractiveBTWMarker();
    
    void draw(QGraphicsScene *scene, WaterfallGraph *graph) override;
    
    /**
     * @brief Get the interactive graphics item
     * @return Pointer to InteractiveGraphicsItem
     */
    InteractiveGraphicsItem* getItem() const { return m_item; }
    
    /**
     * @brief Update bearing rate box display
     */
    void updateBearingRateBox();
    
    /**
     * @brief Remove bearing rate box display
     */
    void removeBearingRateBox();
    
    /**
     * @brief Update marker position based on graph's current time/value mapping
     * @param graph Pointer to the waterfall graph for coordinate mapping
     */
    void updatePosition(WaterfallGraph *graph);

private:
    InteractiveGraphicsItem *m_item;
    QGraphicsScene *m_overlayScene;
    QList<QGraphicsItem*> m_bearingRateItems; // Text label and outline for bearing rate display
    void setupInteractiveItem();
};

#endif // INTERACTIVEBTWMARKER_H

