#ifndef TACTICALSOLUTIONVIEW_H
#define TACTICALSOLUTIONVIEW_H

#include <QWidget>

namespace Ui {
class TacticalSolutionView;
}

class TacticalSolutionView : public QWidget
{
    Q_OBJECT

public:
    explicit TacticalSolutionView(QWidget *parent = nullptr);
    ~TacticalSolutionView();

private:
    Ui::TacticalSolutionView *ui;
};

#endif // TACTICALSOLUTIONVIEW_H
