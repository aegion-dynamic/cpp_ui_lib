#include "maneuverillustration.h"
#include <QDateTime>
#include <QPen>
#include <QFontMetrics>
#include <QDebug>
#include <algorithm>
#include <QtGlobal>

ManeuverIllustration::ManeuverIllustration()
    : m_maneuver(), m_drawArea(), m_chevronWidthPercent(0.4), // 40% of widget width
      m_chevronHeight(8), m_chevronBoxHeight(30)
{
}

ManeuverIllustration::ManeuverIllustration(const QRect& drawArea)
    : m_maneuver(), m_drawArea(drawArea), m_chevronWidthPercent(0.4),
      m_chevronHeight(8), m_chevronBoxHeight(30)
{
}

void ManeuverIllustration::setManeuver(const Maneuver& maneuver)
{
    m_maneuver = maneuver;
}

void ManeuverIllustration::setDrawArea(const QRect& drawArea)
{
    m_drawArea = drawArea;
}

void ManeuverIllustration::update()
{
    // Update internal state if needed
    // This could include recalculating positions, updating animations, etc.
    // For now, we'll keep it simple as the main state is managed externally
}

int ManeuverIllustration::timeToYPosition(const QDateTime& time, int widgetHeight) const
{
    if (!time.isValid() || widgetHeight <= 0)
    {
        return 0;
    }
    
    // Use similar logic to SliderGeometry
    QDateTime now = QDateTime::currentDateTime();
    QDateTime twelveHoursAgo = now.addSecs(-12 * 3600);
    
    // Calculate minutes from twelveHoursAgo to the target time
    int minutesFromStart = twelveHoursAgo.msecsTo(time) / 60000;
    
    // Clamp to valid range (0 to 720 minutes = 12 hours)
    const int TWELVE_HOURS_IN_MINUTES = 720;
    minutesFromStart = qBound(0, minutesFromStart, TWELVE_HOURS_IN_MINUTES);
    
    // Convert to Y position
    // Note: In timeline view, Y=0 is at the top (now), and Y increases downward (past)
    // So we invert the calculation: positionRatio = 1.0 - (minutesFromStart / 720)
    double positionRatio = static_cast<double>(minutesFromStart) / static_cast<double>(TWELVE_HOURS_IN_MINUTES);
    
    // Invert so that top (Y=0) represents "now" and bottom represents "12 hours ago"
    int yPosition = static_cast<int>((1.0 - positionRatio) * widgetHeight);
    
    return yPosition;
}

void ManeuverIllustration::drawDashedStartLine(QPainter& painter, int yPosition)
{
    if (m_drawArea.width() <= 0 || m_drawArea.height() <= 0)
    {
        return;
    }
    
    // Clamp yPosition to visible area
    yPosition = qBound(0, yPosition, m_drawArea.height());
    
    // Draw a dashed blue horizontal line across the full width
    // Use a thinner pen with smaller dashes
    QPen dashedPen(QColor(0, 150, 255), 2); // Brighter blue, 2px width (thinner)
    dashedPen.setStyle(Qt::DashLine);
    dashedPen.setDashPattern({2, 2}); // Smaller dash pattern: 2px dash, 2px gap
    painter.setPen(dashedPen);
    
    // Draw horizontal line at yPosition spanning the full width
    painter.drawLine(0, yPosition, m_drawArea.width(), yPosition);
}

void ManeuverIllustration::drawChevron(QPainter& painter, int yPosition,
                                       const QString& label1,
                                       const QString& label2,
                                       const QString& label3)
{
    if (m_drawArea.width() <= 0 || m_drawArea.height() <= 0)
    {
        return;
    }
    
    // Clamp yPosition to valid range, but allow some margin for chevron box
    int minY = m_chevronBoxHeight + m_chevronHeight;
    int maxY = m_drawArea.height();
    yPosition = qBound(minY, yPosition, maxY);
    
    int widgetWidth = m_drawArea.width();
    
    // Set pen for blue chevron outline
    painter.setPen(QPen(QColor(0, 100, 255), 3)); // Blue color, 3px width for better visibility
    
    // Define chevron size (width and height)
    int chevronWidth = static_cast<int>(widgetWidth * m_chevronWidthPercent);
    
    // Calculate chevron position (centered horizontally)
    int chevronX = (widgetWidth - chevronWidth) / 2;
    // Position chevron so the tip is at yPosition
    // chevronY is the top of the chevron V shape
    int chevronY = yPosition - m_chevronHeight;
    
    // Calculate the tip position (bottom center of V) - should be at yPosition
    int tipX = chevronX + chevronWidth / 2;
    int tipY = chevronY + m_chevronHeight; // This should equal yPosition
    
    // Define chevron points (pointing down: V) and the lines to the edges
    QPoint chevronPoints[8] = {
        QPoint(0, chevronY - m_chevronBoxHeight),           // Start point (top of box)
        QPoint(0, chevronY),                                // Left edge (top of V)
        QPoint(chevronX, chevronY),                         // Top left point of V
        QPoint(tipX, tipY),                                 // Bottom point (tip) at yPosition
        QPoint(chevronX + chevronWidth, chevronY),         // Top right point of V
        QPoint(widgetWidth, chevronY),                      // Right edge (top of V)
        QPoint(widgetWidth, chevronY - m_chevronBoxHeight), // Right edge (top of box)
        QPoint(0, chevronY - m_chevronBoxHeight)            // Start point (top of box)
    };
    
    // Draw the chevron outline
    painter.drawPolygon(chevronPoints, 8);
    
    // Draw the 3 labels - 1 and 3 below the V, 2 at the top
    painter.setPen(QPen(QColor(0, 100, 255), 2)); // Blue text, thicker
    
    // Calculate text metrics for centering
    QFontMetrics fontMetrics(painter.font());
    
    // Label 1: below the chevron, left of the V (centered at chevronX)
    if (!label1.isEmpty())
    {
        int label1Width = fontMetrics.horizontalAdvance(label1);
        int label1X = chevronX - label1Width / 2; // Center at chevronX
        int label1Y = tipY + 15;
        painter.drawText(QPoint(label1X, label1Y), label1);
    }
    
    // Label 2: at the top center (centered at tipX)
    if (!label2.isEmpty())
    {
        int label2Width = fontMetrics.horizontalAdvance(label2);
        int label2X = tipX - label2Width / 2; // Center at tipX
        int label2Y = chevronY;
        painter.drawText(QPoint(label2X, label2Y), label2);
    }
    
    // Label 3: below the chevron, right of the V (centered at chevronX + chevronWidth)
    if (!label3.isEmpty())
    {
        int label3Width = fontMetrics.horizontalAdvance(label3);
        int label3X = (chevronX + chevronWidth) - label3Width / 2; // Center at right edge
        int label3Y = tipY + 15;
        painter.drawText(QPoint(label3X, label3Y), label3);
    }
}

void ManeuverIllustration::draw(QPainter& painter)
{
    if (m_maneuver.isEmpty() || m_drawArea.width() <= 0 || m_drawArea.height() <= 0)
    {
        return;
    }
    
    int widgetHeight = m_drawArea.height();
    
    // Draw chevrons at each step's start time FIRST
    QList<ManeuverStep> steps = m_maneuver.getSteps();
    for (const ManeuverStep& step : steps)
    {
        QDateTime stepStartTime = step.getStartTime();
        if (stepStartTime.isValid())
        {
            int stepY = timeToYPosition(stepStartTime, widgetHeight);
            // Draw chevron even if slightly outside bounds (drawChevron will clamp)
            drawChevron(painter, stepY, 
                       step.getLabel1(), step.getLabel2(), step.getLabel3());
        }
    }
    
    // Draw chevron at maneuver end time
    QDateTime endTime = m_maneuver.getEndTime();
    if (endTime.isValid())
    {
        int endY = timeToYPosition(endTime, widgetHeight);
        // For end chevron, use default labels if no steps provided
        QString label1, label2, label3;
        if (steps.isEmpty())
        {
            // Use default labels
            label1 = "1";
            label2 = "2";
            label3 = "3";
        }
        else
        {
            // Use labels from last step if available
            const ManeuverStep& lastStep = steps.last();
            label1 = lastStep.getLabel1();
            label2 = lastStep.getLabel2();
            label3 = lastStep.getLabel3();
        }
        // Draw chevron even if slightly outside bounds (drawChevron will clamp)
        drawChevron(painter, endY, label1, label2, label3);
    }
    
    // Draw dashed blue line at maneuver start AFTER chevrons so it appears below them
    // The start line should be drawn at the maneuver start time, which is the earliest step time
    QDateTime startTime = m_maneuver.getStartTime();
    if (startTime.isValid())
    {
        int startY = timeToYPosition(startTime, widgetHeight);
        // Draw the start line below the chevron
        // The chevron tip is at startY, labels are at tipY + 15
        // The chevron box extends upward, so we need to draw the line well below the tip
        // to ensure it's clearly visible below the entire chevron including labels
        int lineY = startY + 35; // Well below the chevron tip and labels (15px for labels + 20px spacing)
        // Always draw the start line (drawDashedStartLine will clamp if needed)
        drawDashedStartLine(painter, lineY);
    }
}

