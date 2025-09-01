#ifndef TACTICALSOLUTIONVIEW_H
#define TACTICALSOLUTIONVIEW_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QGraphicsScene>
#include "drawutils.h"

namespace Ui
{
    class TacticalSolutionView;
}

class TacticalSolutionView : public QWidget
{
    Q_OBJECT

public:
    explicit TacticalSolutionView(QWidget *parent = nullptr);
    ~TacticalSolutionView();

protected:
    void paintEvent(QPaintEvent *event) override;
    // void resizeEvent(QResizeEvent *event) override;
    // void mouseMoveEvent(QMouseEvent *event) override;

private:
    // Drawing functions
    void draw();
    void drawBackground();
    void drawTestPattern();

    void drawVectors(QPointF ownShipPosition, QPointF selectedTrackPosition, QPointF adoptedPosition);
    void drawCourseVector(QGraphicsScene* scene, QPointF startPoint, double magnitude, double bearing, const QColor& color);
    

private:
    Ui::TacticalSolutionView *ui;
    QGraphicsScene *scene;
};

#endif // TACTICALSOLUTIONVIEW_H
