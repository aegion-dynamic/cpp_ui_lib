#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QTime>
#include <QList>
#include "timelineutils.h"

// Compile-time parameters
#define TIMELINE_VIEW_BUTTON_SIZE 32
#define TIMELINE_VIEW_GRAPHICS_VIEW_WIDTH 32
#define MAX_TIME_SELECTIONS 5


namespace Ui {
class TimelineView;
}

class TimelineVisualizerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineVisualizerWidget(QWidget *parent = nullptr);
    ~TimelineVisualizerWidget();
    
    // Time selection management
    void addTimeSelection(TimeSelectionSpan span);
    void clearTimeSelections();
        
    // Properties
    void setTimeLineLength(const QTime& length);
    void setCurrentTime(const QTime& currentTime);
    
    QTime getTimeLineLength() const { return m_timeLineLength; }
    QTime getCurrentTime() const { return m_currentTime; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QList<TimeSelectionSpan> m_timeSelections;
    QTime m_timeLineLength;
    QTime m_currentTime;
    
    void updateVisualization();
    void drawSelection(QPainter &painter, const TimeSelectionSpan &span);
};


class TimelineView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineView(QWidget *parent = nullptr);
    ~TimelineView();
    
    // Delegate methods to the visualizer widget
    void addTimeSelection(TimeSelectionSpan span) { m_visualizerWidget->addTimeSelection(span); }
    void clearTimeSelections() { m_visualizerWidget->clearTimeSelections(); }
    void setTimeLineLength(const QTime& length) { m_visualizerWidget->setTimeLineLength(length); }
    void setCurrentTime(const QTime& currentTime) { m_visualizerWidget->setCurrentTime(currentTime); }

private:
    Ui::TimelineView *ui;

    QPushButton *m_button;
    TimelineVisualizerWidget *m_visualizerWidget;
    QVBoxLayout *m_layout;

private slots:
    void onButtonClicked();

};

#endif // TIMELINEVIEW_H
