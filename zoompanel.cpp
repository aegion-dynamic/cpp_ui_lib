#include "zoompanel.h"
#include "ui_zoompanel.h"

ZoomPanel::ZoomPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ZoomPanel)
    , m_scene(nullptr)
    , m_backFrame(nullptr)
    , m_indicator(nullptr)
    , m_leftText(nullptr)
    , m_centerText(nullptr)
    , m_rightText(nullptr)
{
    ui->setupUi(this);
    
    // Set black background
    this->setStyleSheet("background-color: black;");
    
    setupGraphicsView();
}

ZoomPanel::~ZoomPanel()
{
    delete ui;
}

void ZoomPanel::setupGraphicsView()
{
    QRect drawArea = this->rect();

    int sceneWidth = drawArea.width();
    int sceneHeight = drawArea.height();

    // Create graphics scene
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, sceneWidth-1, sceneHeight-1);
    
    // Set black background for the graphics scene
    m_scene->setBackgroundBrush(QBrush(Qt::black));
    
    // Set the scene to the graphics view
    ui->graphicsView->setScene(m_scene);
    
    // Create the visual elements
    // createBackFrame();
    createIndicator();
    createTextItems();
}

void ZoomPanel::createBackFrame()
{
    QRect drawArea = this->rect();
    int frameWidth = drawArea.width() - 10;  // 5px margin on each side
    int frameHeight = drawArea.height() - 10; // 5px margin on each side
    
    // Create light grey inset frame
    m_backFrame = new QGraphicsRectItem(5, 5, frameWidth, frameHeight);
    
    QPen framePen(QColor(200, 200, 200)); // Light grey
    framePen.setWidth(2);
    m_backFrame->setPen(framePen);
    
    QBrush frameBrush(QColor(240, 240, 240)); // Very light grey background
    m_backFrame->setBrush(frameBrush);
    
    m_scene->addItem(m_backFrame);
}

void ZoomPanel::createIndicator()
{
    QRect drawArea = this->rect();
    int indicatorHeight = drawArea.height() - 20; // 10px margin on each side
    int indicatorY = (drawArea.height() - indicatorHeight) / 2; // Center vertically
    int centerX = drawArea.width() / 2; // Center horizontally
    
    // Create rectangular indicator - starts at 2 pixels thin (value 0) centered
    m_indicator = new QGraphicsRectItem(centerX - 1, indicatorY, 2, indicatorHeight);
    
    QPen indicatorPen(QColor(50, 50, 50)); // Dark grey
    indicatorPen.setWidth(1);
    m_indicator->setPen(indicatorPen);
    
    QBrush indicatorBrush(QColor(100, 100, 100)); // Medium grey
    m_indicator->setBrush(indicatorBrush);
    
    m_scene->addItem(m_indicator);
}

void ZoomPanel::createTextItems()
{
    QRect drawArea = this->rect();
    
    // Create font for text
    QFont textFont("Arial", 8);
    
    // Calculate vertical center position
    int textY = (drawArea.height() - 8) / 2; // Center vertically (8 is approximate font height)
    
    // Create left text item
    m_leftText = new QGraphicsTextItem("0.00");
    m_leftText->setFont(textFont);
    m_leftText->setDefaultTextColor(Qt::white);
    m_leftText->setPos(10, textY);
    m_scene->addItem(m_leftText);
    
    // Create center text item
    m_centerText = new QGraphicsTextItem("0.50");
    m_centerText->setFont(textFont);
    m_centerText->setDefaultTextColor(Qt::white);
    m_centerText->setPos(drawArea.width()/2 - 15, textY); // Center horizontally
    m_scene->addItem(m_centerText);
    
    // Create right text item
    m_rightText = new QGraphicsTextItem("1.00");
    m_rightText->setFont(textFont);
    m_rightText->setDefaultTextColor(Qt::white);
    m_rightText->setPos(drawArea.width() - 35, textY); // Right aligned
    m_scene->addItem(m_rightText);
}

void ZoomPanel::setIndicatorValue(double value)
{
    updateIndicator(value);
}

void ZoomPanel::updateIndicator(double value)
{
    if (!m_indicator) return;
    
    // Clamp value between 0.0 and 1.0
    value = qBound(0.0, value, 1.0);
    
    QRect drawArea = this->rect();
    int availableWidth = drawArea.width() - 20; // 10px margin on each side
    int indicatorHeight = drawArea.height() - 20; // 10px margin on each side
    int indicatorY = (drawArea.height() - indicatorHeight) / 2; // Center vertically
    int centerX = drawArea.width() / 2; // Center horizontally
    
    // Calculate width: minimum 2 pixels, maximum available width
    double minWidth = 2.0;
    double maxWidth = static_cast<double>(availableWidth);
    double width = minWidth + (maxWidth - minWidth) * value;
    
    // Calculate X position to center the indicator
    double indicatorX = centerX - (width / 2);
    
    // Update indicator rectangle with centered position
    QRectF rect(indicatorX, indicatorY, width, indicatorHeight);
    m_indicator->setRect(rect);
}

void ZoomPanel::setLeftLabelValue(qreal value)
{
    leftLabelValue = value;
    if (m_leftText) {
        m_leftText->setPlainText(QString::number(value, 'f', 2));
    }
}

void ZoomPanel::setCenterLabelValue(qreal value)
{
    centerLabelValue = value;
    if (m_centerText) {
        m_centerText->setPlainText(QString::number(value, 'f', 2));
    }
}

void ZoomPanel::setRightLabelValue(qreal value)
{
    rightLabelValue = value;
    if (m_rightText) {
        m_rightText->setPlainText(QString::number(value, 'f', 2));
    }
}
