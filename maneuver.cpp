#include "maneuver.h"

// ManeuverStep Implementation

ManeuverStep::ManeuverStep()
    : m_startTime(), m_label1(), m_label2(), m_label3()
{
}

ManeuverStep::ManeuverStep(const QDateTime& startTime,
                           const QString& label1,
                           const QString& label2,
                           const QString& label3)
    : m_startTime(startTime), m_label1(label1), m_label2(label2), m_label3(label3)
{
}

// Maneuver Implementation

Maneuver::Maneuver()
    : m_steps(), m_endTime()
{
}

Maneuver::Maneuver(const QDateTime& endTime)
    : m_steps(), m_endTime(endTime)
{
}

void Maneuver::addStep(const ManeuverStep& step)
{
    m_steps.append(step);
}

void Maneuver::addStep(const QDateTime& startTime,
                       const QString& label1,
                       const QString& label2,
                       const QString& label3)
{
    m_steps.append(ManeuverStep(startTime, label1, label2, label3));
}

void Maneuver::removeStep(int index)
{
    if (index >= 0 && index < m_steps.size())
    {
        m_steps.removeAt(index);
    }
}

void Maneuver::clearSteps()
{
    m_steps.clear();
}

ManeuverStep Maneuver::getStep(int index) const
{
    if (index >= 0 && index < m_steps.size())
    {
        return m_steps.at(index);
    }
    return ManeuverStep(); // Return default step if index is invalid
}

QDateTime Maneuver::getStartTime() const
{
    if (m_steps.isEmpty())
    {
        return m_endTime;
    }
    
    QDateTime earliestTime = m_steps.first().getStartTime();
    for (const ManeuverStep& step : m_steps)
    {
        if (step.getStartTime().isValid() && step.getStartTime() < earliestTime)
        {
            earliestTime = step.getStartTime();
        }
    }
    return earliestTime;
}

