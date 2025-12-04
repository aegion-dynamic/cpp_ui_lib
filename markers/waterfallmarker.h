#ifndef WATERFALLMARKER_H
#define WATERFALLMARKER_H

#include <QDateTime>
#include <QGraphicsScene>
#include <QString>
#include <memory>

class WaterfallGraph;

/**
 * @brief Base class for all waterfall graph markers
 * 
 * Provides unified interface for interactive and non-interactive markers
 * with hash-based identification and drawing capabilities.
 */
class WaterfallMarker
{
public:
    /**
     * @brief Marker type enumeration
     */
    enum class MarkerType {
        RTWRMarker,          ///< RTW R marker (yellow "R" text)
        RTWSymbolMarker,    ///< RTW symbol marker (pixmap symbols)
        BTWCircleMarker,    ///< BTW circle marker with angle line
        InteractiveBTWMarker ///< Interactive BTW marker (draggable)
    };

    /**
     * @brief Constructor
     * @param type Marker type
     * @param timestamp Timestamp for the marker
     * @param value Value (range) for the marker
     */
    WaterfallMarker(MarkerType type, const QDateTime &timestamp, qreal value);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~WaterfallMarker() = default;

    /**
     * @brief Draw the marker on the given scene
     * @param scene Graphics scene to draw on
     * @param graph Pointer to the waterfall graph (for coordinate mapping)
     */
    virtual void draw(QGraphicsScene *scene, WaterfallGraph *graph) = 0;

    /**
     * @brief Get the hash string for this marker
     * Hash is based on type + value + timestamp (rounded to seconds)
     * @return Hash string
     */
    QString getHash() const;

    /**
     * @brief Get marker type
     * @return Marker type
     */
    MarkerType getType() const { return m_type; }

    /**
     * @brief Get timestamp
     * @return Timestamp
     */
    QDateTime getTimestamp() const { return m_timestamp; }

    /**
     * @brief Get value (range)
     * @return Value
     */
    qreal getValue() const { return m_value; }

    /**
     * @brief Check if marker is interactive
     * @return True if interactive, false otherwise
     */
    virtual bool isInteractive() const { return false; }

protected:
    MarkerType m_type;
    QDateTime m_timestamp;
    qreal m_value;
    QString m_hash;
};

/**
 * @brief Base class for non-interactive markers
 */
class NonInteractiveWaterfallMarker : public WaterfallMarker
{
public:
    NonInteractiveWaterfallMarker(MarkerType type, const QDateTime &timestamp, qreal value);
    virtual ~NonInteractiveWaterfallMarker() = default;
    
    bool isInteractive() const override { return false; }
};

/**
 * @brief Base class for interactive markers
 */
class InteractiveWaterfallMarker : public WaterfallMarker
{
public:
    InteractiveWaterfallMarker(MarkerType type, const QDateTime &timestamp, qreal value);
    virtual ~InteractiveWaterfallMarker() = default;
    
    bool isInteractive() const override { return true; }
};

#endif // WATERFALLMARKER_H

