#ifndef RTWRMARKER_H
#define RTWRMARKER_H

#include "waterfallmarker.h"
#include <QDateTime>

/**
 * @brief RTW R marker - draws yellow "R" text
 */
class RTWRMarker : public NonInteractiveWaterfallMarker
{
public:
    RTWRMarker(const QDateTime &timestamp, qreal range);
    virtual ~RTWRMarker() = default;
    
    void draw(QGraphicsScene *scene, WaterfallGraph *graph) override;
};

#endif // RTWRMARKER_H

