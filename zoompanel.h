#ifndef ZOOMPANEL_H
#define ZOOMPANEL_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
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

private:
    Ui::ZoomPanel *ui;
    QGraphicsScene *m_scene;
    QGraphicsRectItem *m_backFrame;
    QGraphicsRectItem *m_indicator;
    
    void setupGraphicsView();
    void createBackFrame();
    void createIndicator();
    void updateIndicator(double value);
};

#endif // ZOOMPANEL_H
