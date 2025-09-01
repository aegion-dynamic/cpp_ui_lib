#ifndef TWOAXISGRAPH_H
#define TWOAXISGRAPH_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QGraphicsScene>

namespace Ui {
class twoaxisgraph;
}

class twoaxisgraph : public QWidget
{
    Q_OBJECT

public:
    explicit twoaxisgraph(QWidget *parent = nullptr);
    ~twoaxisgraph();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // Drawing functions
    void draw();
    void drawBackground();
    void drawGraphArea();
    void drawTestPattern();
    void drawInfoArea();
    void drawAxesLabels();

private:
    Ui::twoaxisgraph *ui;
    QGraphicsScene *scene;

    // Utility functions
    QRectF getGraphDrawArea() const;

};
#endif // TWOAXISGRAPH_H
