#include "twoaxisgraph.h"
#include "ui_twoaxisgraph.h"
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>

twoaxisgraph::twoaxisgraph(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::twoaxisgraph)
    , scene(nullptr)
    , view(nullptr)
{
    ui->setupUi(this);
    
    // Set black background
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);
    
    // Set minimum size
    setMinimumSize(400, 300);

    // Initialize scene and view
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setBackgroundBrush(Qt::black);

    // Add view to layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    setupScene();
}

twoaxisgraph::~twoaxisgraph()
{
    delete ui;
}

void twoaxisgraph::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Set up pen for axis drawing
    QPen axisPen(Qt::white, 2);
    axisPen.setCapStyle(Qt::RoundCap);
    painter.setPen(axisPen);
    
    // Debug output
    qDebug() << "Drawing dimensions:";
    qDebug() << "Widget size:" << width() << "x" << height();
}

void twoaxisgraph::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (view && scene) {
        setupScene();
        // Debug information
        qDebug() << "Widget resized to:" << size();
        qDebug() << "View size:" << view->size();
        qDebug() << "Scene rect:" << scene->sceneRect();

    }
    update();
}

void twoaxisgraph::setupScene()
{
    if (!scene || !view) return;

    // Clear existing items
    scene->clear();

    // Set the scene rect to match widget size
    scene->setSceneRect(0, 0, width(), height());
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // Add basic scene elements
    QPen axisPen(Qt::white, 2);
    scene->addLine(0, 0, width(), height(), axisPen);  // Example line
}

