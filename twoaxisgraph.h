#ifndef TWOAXISGRAPH_H
#define TWOAXISGRAPH_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>

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

private:
    Ui::twoaxisgraph *ui;
    QGraphicsScene *scene;
    QGraphicsView *view;
};

#endif // TWOAXISGRAPH_H
