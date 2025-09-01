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

    void drawVectors();
    void drawOwnShipVector(qreal magnitude, qreal bearing);
    void drawSelectedTrackVector(qreal sensorBearing, qreal selectedTrackDistance, qreal selectedTrackBearing, qreal magnitude);
    void drawAdoptedTrackVector(qreal sensorBearing, qreal adoptedTrackDistance,  qreal adoptedTrackBearing, qreal magnitude);

private:
    Ui::TacticalSolutionView *ui;
    QGraphicsScene *scene;
};

#endif // TACTICALSOLUTIONVIEW_H
