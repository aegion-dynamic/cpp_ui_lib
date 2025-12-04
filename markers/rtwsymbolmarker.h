#ifndef RTWSYMBOLMARKER_H
#define RTWSYMBOLMARKER_H

#include "waterfallmarker.h"
#include <QDateTime>
#include <QString>

/**
 * @brief RTW Symbol marker - draws pixmap symbols (TM, DP, LY, etc.)
 */
class RTWSymbolMarker : public NonInteractiveWaterfallMarker
{
public:
    RTWSymbolMarker(const QString &symbolName, const QDateTime &timestamp, qreal range);
    virtual ~RTWSymbolMarker() = default;
    
    void draw(QGraphicsScene *scene, WaterfallGraph *graph) override;
    
    /**
     * @brief Get symbol name
     * @return Symbol name
     */
    QString getSymbolName() const { return m_symbolName; }

private:
    QString m_symbolName;
};

#endif // RTWSYMBOLMARKER_H

