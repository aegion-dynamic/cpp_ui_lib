#include "waterfallmarker.h"
#include "../waterfallgraph.h"
#include <QDebug>

// WaterfallMarker implementation
WaterfallMarker::WaterfallMarker(MarkerType type, const QDateTime &timestamp, qreal value)
    : m_type(type)
    , m_timestamp(timestamp)
    , m_value(value)
{
    // Generate hash: type + value + timestamp (rounded to seconds)
    qint64 timestampSecs = timestamp.toSecsSinceEpoch();
    uint hashValue = qHash(static_cast<int>(type)) ^ qHash(value) ^ qHash(timestampSecs);
    m_hash = QString::number(hashValue, 16);
}

QString WaterfallMarker::getHash() const
{
    return m_hash;
}

// NonInteractiveWaterfallMarker implementation
NonInteractiveWaterfallMarker::NonInteractiveWaterfallMarker(MarkerType type, const QDateTime &timestamp, qreal value)
    : WaterfallMarker(type, timestamp, value)
{
}

// InteractiveWaterfallMarker implementation
InteractiveWaterfallMarker::InteractiveWaterfallMarker(MarkerType type, const QDateTime &timestamp, qreal value)
    : WaterfallMarker(type, timestamp, value)
{
}

