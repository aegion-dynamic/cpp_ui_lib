#include "twoaxisgraph.h"
#include "ui_twoaxisgraph.h"

twoaxisgraph::twoaxisgraph(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::twoaxisgraph)
{
    ui->setupUi(this);
}

twoaxisgraph::~twoaxisgraph()
{
    delete ui;
}
