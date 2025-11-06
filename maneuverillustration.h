#ifndef MANEUVERILLUSTRATION_H
#define MANEUVERILLUSTRATION_H

#include "maneuver.h"
#include "timelineutils.h"
#include <QPainter>
#include <QRect>

/**
 * Draws a maneuver illustration with chevrons and dashed start line
 * Converts maneuver times to Y positions based on timeline geometry
 */
class ManeuverIllustration
{
public:
    ManeuverIllustration();
    ManeuverIllustration(const QRect& drawArea);
    
    // Maneuver management
    void setManeuver(const Maneuver& maneuver);
    Maneuver getManeuver() const { return m_maneuver; }
    
    // Drawing area management
    void setDrawArea(const QRect& drawArea);
    QRect getDrawArea() const { return m_drawArea; }
    
    // Drawing configuration
    void setChevronWidthPercent(double widthPercent) { m_chevronWidthPercent = widthPercent; }
    double getChevronWidthPercent() const { return m_chevronWidthPercent; }
    void setChevronHeight(int height) { m_chevronHeight = height; }
    int getChevronHeight() const { return m_chevronHeight; }
    void setChevronBoxHeight(int height) { m_chevronBoxHeight = height; }
    int getChevronBoxHeight() const { return m_chevronBoxHeight; }
    
    // Update and draw
    void update();
    void draw(QPainter& painter);
    
private:
    Maneuver m_maneuver;
    QRect m_drawArea;
    
    // Chevron drawing parameters
    double m_chevronWidthPercent; // Percentage of widget width
    int m_chevronHeight;
    int m_chevronBoxHeight;
    
    // Time-to-position conversion (similar to SliderGeometry)
    int timeToYPosition(const QDateTime& time, int widgetHeight) const;
    
    // Drawing methods
    void drawDashedStartLine(QPainter& painter, int yPosition);
    void drawChevron(QPainter& painter, int yPosition, const QString& label1, 
                     const QString& label2, const QString& label3);
};

#endif // MANEUVERILLUSTRATION_H

