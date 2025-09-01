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
    // Scene management
    void setupScene();
    void renderScene(QPainter *painter);

private:
    Ui::twoaxisgraph *ui;
    QGraphicsScene *scene;

    // Utility functions
    QRectF getGraphDrawArea() const;

};
#endif // TWOAXISGRAPH_H
