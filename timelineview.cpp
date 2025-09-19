#include "timelineview.h"
#include "ui_timelineview.h"
#include <QDebug>

TimelineVisualizerWidget::TimelineVisualizerWidget(QWidget *parent)
    : QWidget(parent)
    , m_timeLineLength(QTime(0, 0, 0))
    , m_currentTime(QTime(0, 0, 0))
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

void TimelineVisualizerWidget::updateVisualization()
{
    update(); // Trigger a repaint
}

TimelineVisualizerWidget::~TimelineVisualizerWidget()
{
}

void TimelineVisualizerWidget::addTimeSelection(TimeSelectionSpan span)
{
    if (m_timeSelections.size() < MAX_TIME_SELECTIONS) {
        m_timeSelections.append(span);
        updateVisualization();
    }
}

void TimelineVisualizerWidget::clearTimeSelections()
{
    m_timeSelections.clear();
    updateVisualization();
}

void TimelineVisualizerWidget::drawSelection(QPainter &painter, const TimeSelectionSpan &span)
{
    // First get the draw area
    QRect drawArea = rect();
    int widgetHeight = drawArea.height();
    int widgetWidth = drawArea.width();

    // Calculate the total timeline duration in seconds
    int totalSeconds = m_timeLineLength.hour() * 3600 + m_timeLineLength.minute() * 60 + m_timeLineLength.second();
    
    if (totalSeconds <= 0 || widgetHeight <= 0) {
        return; // Invalid parameters
    }

    // Calculate pixels per second
    double pixelsPerSecond = static_cast<double>(widgetHeight) / totalSeconds;

    // Get current time and selection times in seconds
    int currentTimeSeconds = m_currentTime.hour() * 3600 + m_currentTime.minute() * 60 + m_currentTime.second();
    int selectionStartSeconds = span.startTime.hour() * 3600 + span.startTime.minute() * 60 + span.startTime.second();
    int selectionEndSeconds = span.endTime.hour() * 3600 + span.endTime.minute() * 60 + span.endTime.second();

    // Calculate the visible time range (currentTime is at top, currentTime-timespan is at bottom)
    int timeSpanStartSeconds = currentTimeSeconds - totalSeconds;

    // Check if selection overlaps with visible range
    if (selectionEndSeconds >= timeSpanStartSeconds && selectionStartSeconds <= currentTimeSeconds) {
        // Calculate Y positions relative to currentTime (top of widget)
        int topY = static_cast<int>((currentTimeSeconds - selectionEndSeconds) * pixelsPerSecond);
        int bottomY = static_cast<int>((currentTimeSeconds - selectionStartSeconds) * pixelsPerSecond);

        // Clamp to widget bounds
        topY = qMax(0, qMin(widgetHeight, topY));
        bottomY = qMax(0, qMin(widgetHeight, bottomY));

        // Ensure the rectangle is at least 1 pixel high
        int rectHeight = qMax(1, bottomY - topY);

        // Now draw the selection
        painter.fillRect(0, topY, widgetWidth, rectHeight, QColor(255, 255, 255));

        // Now draw the border
        painter.setPen(QPen(QColor(150, 150, 150), 1));
        painter.drawRect(0, topY, widgetWidth, rectHeight);
    }
}

void TimelineVisualizerWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Fill with light grey background
    painter.fillRect(rect(), QColor(200, 200, 200));
    
    // Draw time selection rectangles
    if (!m_timeSelections.isEmpty() && !m_timeLineLength.isNull() && !m_currentTime.isNull()) {
        for (const TimeSelectionSpan& span : m_timeSelections) {
            drawSelection(painter, span);
        }
    }
    
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
    m_button = new QPushButton("H", this);
    m_button->setFixedSize(TIMELINE_VIEW_BUTTON_SIZE, TIMELINE_VIEW_BUTTON_SIZE);
    m_button->setStyleSheet(
        "QPushButton {"
        "    background-color: grey;"
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
}

TimelineView::~TimelineView()
{
    delete ui;
}

void TimelineView::onButtonClicked()
{
    clearTimeSelections();
    qDebug() << "Time selections cleared!";
}
