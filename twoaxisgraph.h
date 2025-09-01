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

    // Set the percentage of events to drop (0-100)
    void setEventDropPercentage(int percentage) {
        dropPercentage = qBound(0, percentage, 100);
    }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    // Drawing functions
    void draw();
    void drawBackground();
    void drawGraphArea();
    void drawAxes();  // New method for drawing axes
    void drawTestPattern();
    void drawInfoArea();
    void drawAxesLabels();
    void drawCursor();

private:
    Ui::twoaxisgraph *ui;
    QGraphicsScene *scene;
    QPoint currentMousePos;  // Store current mouse position

    // Debounce settings
    int eventCount = 0;
    int dropPercentage = 50;  // Drop 80% of events by default
    bool shouldProcessEvent();

    // Utility functions
    QRectF getGraphDrawArea() const;
    QPointF getSceneCoordinates(const QPoint& widgetPos) const;
    
    // Text rendering
    QGraphicsTextItem* createAxisLabel(const QString& text, 
                                     const QColor& textColor,
                                     const QColor& borderColor,
                                     const QColor& backgroundColor);

};
#endif // TWOAXISGRAPH_H
