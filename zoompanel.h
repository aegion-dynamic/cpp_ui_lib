#ifndef ZOOMPANEL_H
#define ZOOMPANEL_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QThread>

namespace Ui {
class ZoomPanel;
}

class ZoomPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ZoomPanel(QWidget *parent = nullptr);
    ~ZoomPanel();

    void setIndicatorValue(double value);
    void testIndicator(); // Test method to demonstrate functionality
    
    // Label value setters
    void setLeftLabelValue(qreal value);
    void setCenterLabelValue(qreal value);
    void setRightLabelValue(qreal value);

private:
    Ui::ZoomPanel *ui;
    QGraphicsScene *m_scene;
    QGraphicsRectItem *m_backFrame;
    QGraphicsRectItem *m_indicator;
    QGraphicsTextItem *m_leftText;
    QGraphicsTextItem *m_centerText;
    QGraphicsTextItem *m_rightText;
    
    void setupGraphicsView();
    void createBackFrame();
    void createIndicator();
    void createTextItems();
    void updateIndicator(double value);
};

#endif // ZOOMPANEL_H
