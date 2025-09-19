#include "timelineview.h"
#include "ui_timelineview.h"
#include <QDebug>

TimelineVisualizerWidget::TimelineVisualizerWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentTime(QTime::currentTime())
    , m_numberOfDivisions(15)
{
    setFixedWidth(TIMELINE_VIEW_GRAPHICS_VIEW_WIDTH);
    setMinimumHeight(50); // Set a minimum height
}

void TimelineVisualizerWidget::setTimeLineLength(const QTime& length)
{
    m_timeLineLength = length;
    updateVisualization();
}

void TimelineVisualizerWidget::setCurrentTime(const QTime& currentTime)
{
    m_currentTime = currentTime;
    updateVisualization();
}

void TimelineVisualizerWidget::setNumberOfDivisions(int divisions)
{
    m_numberOfDivisions = divisions;
    updateVisualization();
}

void TimelineVisualizerWidget::updateVisualization()
{
    update(); // Trigger a repaint
}

TimelineVisualizerWidget::~TimelineVisualizerWidget()
{
}

// No time selection methods needed for TimelineView

// No drawSelection method needed for TimelineView

QString TimelineVisualizerWidget::getTimeLabel(int segmentNumber)
{
    QString timestamp;
    if (!m_timeLineLength.isNull() && !m_currentTime.isNull()) {
        // Calculate the time interval per segment
        int totalSeconds = m_timeLineLength.hour() * 3600 + m_timeLineLength.minute() * 60 + m_timeLineLength.second();
        int segmentIntervalSeconds = totalSeconds / m_numberOfDivisions;
        
        // Subtract segmentNumber * segmentInterval from currentTime and set label in HH:MM format
        QTime segmentTime = m_currentTime.addSecs(-segmentNumber * segmentIntervalSeconds);
        
        // Handle case where time goes into previous day - wrap around
        if (segmentTime.isNull()) {
            // If addSecs resulted in invalid time, try adding to start of day
            int currentSeconds = m_currentTime.hour() * 3600 + m_currentTime.minute() * 60 + m_currentTime.second();
            int targetSeconds = currentSeconds - (segmentNumber * segmentIntervalSeconds);
            
            // Wrap around if negative
            if (targetSeconds < 0) {
                targetSeconds += 24 * 3600; // Add 24 hours
            }
            
            int hours = targetSeconds / 3600;
            int minutes = (targetSeconds % 3600) / 60;
            segmentTime = QTime(hours, minutes, 0);
        }
        
        timestamp = segmentTime.toString("HH:mm");
    }
    return timestamp;
}

void TimelineVisualizerWidget::drawSegment(QPainter &painter, int segmentNumber)
{
    QRect drawArea = rect();
    int widgetHeight = drawArea.height();
    int widgetWidth = drawArea.width();
    
    if (m_numberOfDivisions <= 0 || widgetHeight <= 0) {
        return; // Invalid parameters
    }
    
    // Calculate segment height
    double segmentHeight = static_cast<double>(widgetHeight) / m_numberOfDivisions;
    
    // Calculate Y position for this segment
    double y = segmentNumber * segmentHeight;
    int segmentY = static_cast<int>(y);
    int segmentH = static_cast<int>(segmentHeight);    
    
    // Draw segment border
    painter.setPen(QPen(QColor(100, 100, 100), 1));
    painter.drawLine(0, segmentY, widgetWidth, segmentY);
    
    // Calculate timestamp for this segment
    QString timestamp = getTimeLabel(segmentNumber);
    if (timestamp.isNull()) {
        return; // No timestamp to draw, but segment background is already drawn
    }
    
    // Set text color to white for visibility on dark background
    painter.setPen(QPen(QColor(255, 255, 255), 1));
    
    // Calculate text metrics
    QFontMetrics fm(painter.font());
    int textWidth = fm.horizontalAdvance(timestamp);
    int textHeight = fm.height();
    
    // Calculate center position for the text within the segment
    int centerX = (widgetWidth - textWidth) / 2;
    int centerY = static_cast<int>(y + segmentHeight / 2 + textHeight / 2);
    
    // Draw the timestamp centered in the segment
    painter.drawText(QPoint(centerX, centerY), timestamp);


    // Draw two ticks which are 15% of the segment width aligned with the text center and to the left and right edges at centerY
    int tickWidth = static_cast<int>(widgetWidth * 0.15);

    // Left tick startpoint at left edge at center
    int tickY = centerY; //static_cast<int>(y + segmentHeight / 2);
    QPoint leftTickStart(0, tickY);
    QPoint leftTickEnd(tickWidth, tickY);
    painter.drawLine(leftTickStart, leftTickEnd);

    // Right tick startpoint at right edge at center
    QPoint rightTickStart(widgetWidth, tickY);
    QPoint rightTickEnd(widgetWidth - tickWidth, tickY);
    painter.drawLine(rightTickStart, rightTickEnd);

        


}

void TimelineVisualizerWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Fill with black background
    painter.fillRect(rect(), QColor(0, 0, 0));
    
    // Draw segments
    for (int i = 0; i < m_numberOfDivisions; ++i) {
        drawSegment(painter, i);
    }
    
    // No time selections to draw in TimelineView
    
    // Draw a border to make it more visible
    painter.setPen(QPen(QColor(150, 150, 150), 1));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}



TimelineView::TimelineView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimelineView)
    , m_button(nullptr)
    , m_visualizerWidget(nullptr)
    , m_layout(nullptr)
{
    ui->setupUi(this);

    // Create vertical layout
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    
    // Create button with grey background and white border
    m_button = new QPushButton("Abs:\ndt: 00:15", this);
    m_button->setFixedSize(TIMELINE_VIEW_GRAPHICS_VIEW_WIDTH, TIMELINE_VIEW_BUTTON_SIZE);
    m_button->setStyleSheet(
        "QPushButton {"
        "    background-color: black;"
        "    border: 2px solid white;"
        "    color: white;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: darkgrey;"
        "}"
        "QPushButton:pressed {"
        "    background-color: dimgrey;"
        "}"
    );
    
    // Create visualizer widget
    m_visualizerWidget = new TimelineVisualizerWidget(this);
    
    // Add widgets to layout
    m_layout->addWidget(m_button);
    m_layout->addWidget(m_visualizerWidget, 1); // Stretch factor of 1 to fill remaining space
    
    // Connect button click to internal handler
    connect(m_button, &QPushButton::clicked, this, &TimelineView::onButtonClicked);
    
    // Set the layout
    setLayout(m_layout);
    
    // Set the TimelineView widget width to match button and graphics view width
    setFixedWidth(TIMELINE_VIEW_GRAPHICS_VIEW_WIDTH);

    // Set the default time interval to be 15 minutes
    this->setTimeLineLength(TimeInterval::FifteenMinutes);
    
    // Initialize button text with default interval
    updateButtonText(TimeInterval::FifteenMinutes);

    
}

TimelineView::~TimelineView()
{
    delete ui;
}

void TimelineView::updateButtonText(TimeInterval interval)
{
    QTime timeInterval = timeIntervalToQTime(interval);
    QString timeString = timeInterval.toString("HH:mm");
    QString buttonText = QString("Abs:\ndt: %1").arg(timeString);
    m_button->setText(buttonText);
}

void TimelineView::onButtonClicked()
{
    qDebug() << "TimelineView button clicked!";
    // Cycle through valid time intervals on each button click
    static const std::vector<TimeInterval> intervals = getValidTimeIntervals();
    static int intervalIndex = 0;
    
    // Advance to next interval
    intervalIndex = (intervalIndex + 1) % intervals.size();
    qDebug() << "Interval index:" << intervalIndex;
    qDebug() << "Intervals size:" << intervals.size();
    qDebug() << "Interval:" << static_cast<int>(intervals[intervalIndex]) << "minutes";
    TimeInterval nextInterval = intervals[intervalIndex];
    this->setTimeLineLength(nextInterval);
    // m_visualizerWidget->update(); // Force redraw/repaint
    updateButtonText(nextInterval);
}

