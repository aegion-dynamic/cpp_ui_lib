#ifndef MANEUVER_H
#define MANEUVER_H

#include <QDateTime>
#include <QString>
#include <QList>

/**
 * Represents a single step in a maneuver
 */
class ManeuverStep
{
public:
    ManeuverStep();
    ManeuverStep(const QDateTime& startTime, 
                 const QString& label1 = QString(),
                 const QString& label2 = QString(),
                 const QString& label3 = QString());
    
    // Getters
    QDateTime getStartTime() const { return m_startTime; }
    QString getLabel1() const { return m_label1; }
    QString getLabel2() const { return m_label2; }
    QString getLabel3() const { return m_label3; }
    
    // Setters
    void setStartTime(const QDateTime& startTime) { m_startTime = startTime; }
    void setLabel1(const QString& label) { m_label1 = label; }
    void setLabel2(const QString& label) { m_label2 = label; }
    void setLabel3(const QString& label) { m_label3 = label; }
    
private:
    QDateTime m_startTime;
    QString m_label1;
    QString m_label2;
    QString m_label3;
};

/**
 * Represents a maneuver with one or more steps and an end time
 */
class Maneuver
{
public:
    Maneuver();
    Maneuver(const QDateTime& endTime);
    
    // Step management
    void addStep(const ManeuverStep& step);
    void addStep(const QDateTime& startTime,
                 const QString& label1 = QString(),
                 const QString& label2 = QString(),
                 const QString& label3 = QString());
    void removeStep(int index);
    void clearSteps();
    
    // Getters
    QList<ManeuverStep> getSteps() const { return m_steps; }
    int getStepCount() const { return m_steps.size(); }
    ManeuverStep getStep(int index) const;
    QDateTime getEndTime() const { return m_endTime; }
    QDateTime getStartTime() const; // Returns the earliest step start time, or endTime if no steps
    
    // Setters
    void setEndTime(const QDateTime& endTime) { m_endTime = endTime; }
    
    // Utility
    bool isEmpty() const { return m_steps.isEmpty() && !m_endTime.isValid(); }
    bool hasSteps() const { return !m_steps.isEmpty(); }
    
private:
    QList<ManeuverStep> m_steps;
    QDateTime m_endTime;
};

#endif // MANEUVER_H

