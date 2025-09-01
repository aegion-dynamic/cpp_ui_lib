#ifndef TWOAXISGRAPH_H
#define TWOAXISGRAPH_H

#include <QWidget>

namespace Ui {
class twoaxisgraph;
}

class twoaxisgraph : public QWidget
{
    Q_OBJECT

public:
    explicit twoaxisgraph(QWidget *parent = nullptr);
    ~twoaxisgraph();

private:
    Ui::twoaxisgraph *ui;
};

#endif // TWOAXISGRAPH_H
