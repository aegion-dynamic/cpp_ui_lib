#include "twoaxisgraph.h"
#include "ui_twoaxisgraph.h"

twoaxisgraph::twoaxisgraph(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::twoaxisgraph)
    , scene(nullptr)
{
    ui->setupUi(this);
    
    // Set black background
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);
    
    // Set minimum size
    setMinimumSize(400, 300);

    // Initialize scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, width(), height());
    
    // Make sure widget expands
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    
    // Render the scene
    if (scene) {
        renderScene(&painter);
    }
    
    qDebug() << "Paint event - Widget size:" << width() << "x" << height();
}


void twoaxisgraph::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (scene) {
        // Update scene with new size
        setupScene();
        
        // Debug information
        qDebug() << "Resize event:";
        // qDebug() << "  New widget size:" << event->size();
        // qDebug() << "  Scene rect:" << scene->sceneRect();
    }
    update();
}

void twoaxisgraph::setupScene()
{
    if (!scene) return;

    // Clear existing items
    scene->clear();

    // Update scene rect to match widget size
    scene->setSceneRect(0, 0, width(), height());
    
    // Add basic scene elements
    QPen axisPen(Qt::white, 2);
    
    // Draw test rectangle to show bounds
    scene->addRect(scene->sceneRect(), axisPen);
    
    // Draw diagonal line to show extent
    scene->addLine(0, 0, width(), height(), axisPen);

    // Draw graph area
    scene->addRect(getGraphDrawArea(), axisPen);

    // Debug output
    qDebug() << "Scene setup - Scene rect:" << scene->sceneRect();
}

void twoaxisgraph::renderScene(QPainter *painter)
{
    if (!scene || !painter) return;
    
    // Render the scene directly to the widget
    scene->render(painter, rect(), scene->sceneRect());
}


QRectF twoaxisgraph::getGraphDrawArea() const
{
    //
    if (!scene->sceneRect().isValid()) {
        return QRectF();
    }

    // This is the area where we reserve to draw the graph
    QRectF sceneRect = scene->sceneRect();

    if (!sceneRect.isValid()) {
        return QRectF();
    }

    // This is the area where we reserve to draw the graph
    QRectF graphArea = sceneRect;

    // Reserve 20% of each side for labels/info
    qreal hMargin = graphArea.width() * 0.10;
    qreal vMargin = graphArea.height() * 0.10;

    graphArea.adjust(hMargin, vMargin, -hMargin, -vMargin);
    return graphArea;

}
