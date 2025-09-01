#include "tacticalsolutionview.h"
#include "ui_tacticalsolutionview.h"

TacticalSolutionView::TacticalSolutionView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TacticalSolutionView)
{
    ui->setupUi(this);
}

TacticalSolutionView::~TacticalSolutionView()
{
    delete ui;
}
