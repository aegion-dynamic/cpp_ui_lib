#ifndef BTWCIRCLEMARKER_H
#define BTWCIRCLEMARKER_H

#include "waterfallmarker.h"
#include <QDateTime>

/**
 * @brief BTW Circle marker - draws circle with angle line
 */
class BTWCircleMarker : public NonInteractiveWaterfallMarker
{
public:
    BTWCircleMarker(const QDateTime &timestamp, qreal range, qreal delta);
    virtual ~BTWCircleMarker() = default;
    
    void draw(QGraphicsScene *scene, WaterfallGraph *graph) override;
    
    /**
     * @brief Get delta value
     * @return Delta value
     */
    qreal getDelta() const { return m_delta; }

private:
    qreal m_delta;
};

#endif // BTWCIRCLEMARKER_H

