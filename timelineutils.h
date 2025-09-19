#ifndef TIMELINEUTILS_H
#define TIMELINEUTILS_H

#include <QTime>

struct TimeSelectionSpan {
    QTime startTime;
    QTime endTime;
    
    TimeSelectionSpan() = default;
    TimeSelectionSpan(const QTime& start, const QTime& end) : startTime(start), endTime(end) {}
};

#endif // TIMELINEUTILS_H