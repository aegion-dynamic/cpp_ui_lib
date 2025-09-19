#include "timelineview.h"
#include "ui_timelineview.h"

TimelineView::TimelineView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimelineView)
{
    ui->setupUi(this);
}

TimelineView::~TimelineView()
{
    delete ui;
}
