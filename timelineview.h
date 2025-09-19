#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QWidget>

namespace Ui {
class TimelineView;
}

class TimelineView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineView(QWidget *parent = nullptr);
    ~TimelineView();

private:
    Ui::TimelineView *ui;
};

#endif // TIMELINEVIEW_H
