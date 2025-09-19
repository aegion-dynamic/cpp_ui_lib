#include "zoompanel.h"
#include "ui_zoompanel.h"

ZoomPanel::ZoomPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ZoomPanel)
    , m_scene(nullptr)
    , m_indicator(nullptr)
    , m_leftText(nullptr)
    , m_centerText(nullptr)
    , m_rightText(nullptr)
    , m_isDragging(false)
    , m_currentValue(0.5)
    , m_startedFromRightHalf(false)
{
    ui->setupUi(this);
    
    // Set black background
    this->setStyleSheet("background-color: black;");
    
    // Enable mouse tracking for drag operations
    this->setMouseTracking(true);
    
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
    
    // Ensure graphics view doesn't interfere with mouse events
    ui->graphicsView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    
    // Create the visual elements
    createIndicator();
    createTextItems();
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
    m_currentValue = value;
    updateIndicator(value);
    
    // Calculate bounds: upperbound = center + value*|right-center|, lowerbound = center - value*|center-left|
    ZoomBounds bounds;
    bounds.upperbound = centerLabelValue + value * qAbs(rightLabelValue - centerLabelValue);
    bounds.lowerbound = centerLabelValue - value * qAbs(centerLabelValue - leftLabelValue);
    
    emit valueChanged(bounds);
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

void ZoomPanel::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse press event received at:" << event->pos();
    
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_initialMousePos = event->pos();
        
        // Determine if started from right half
        QRect drawArea = this->rect();
        int centerX = drawArea.width() / 2;
        m_startedFromRightHalf = (event->pos().x() > centerX);
        
        qDebug() << "Started dragging from" << (m_startedFromRightHalf ? "right" : "left") << "half";
        
        // Set cursor to indicate dragging
        setCursor(Qt::ClosedHandCursor);
    }
    
    QWidget::mousePressEvent(event);
}

void ZoomPanel::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        qDebug() << "Mouse move while dragging at:" << event->pos();
        updateValueFromMousePosition(event->pos());
    }
    
    QWidget::mouseMoveEvent(event);
}

void ZoomPanel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
    
    QWidget::mouseReleaseEvent(event);
}

void ZoomPanel::updateValueFromMousePosition(const QPoint &currentPos)
{
    QRect drawArea = this->rect();
    int centerX = drawArea.width() / 2;
    
    // Calculate horizontal movement
    int deltaX = currentPos.x() - m_initialMousePos.x();
    
    qDebug() << "DeltaX:" << deltaX << "Current value:" << m_currentValue;
    
    // Determine value change based on starting position and movement
    qreal valueChange = 0.0;
    
    if (m_startedFromRightHalf) {
        // Started from right half
        if (deltaX > 0) {
            // Moving right - increase value
            valueChange = static_cast<qreal>(deltaX) / static_cast<qreal>(drawArea.width()) * 0.2; // Reduced scale factor
        } else {
            // Moving left - decrease value
            valueChange = static_cast<qreal>(deltaX) / static_cast<qreal>(drawArea.width()) * 0.2; // Reduced scale factor
        }
    } else {
        // Started from left half - reverse the logic
        if (deltaX > 0) {
            // Moving right - decrease value (opposite behavior)
            valueChange = -static_cast<qreal>(deltaX) / static_cast<qreal>(drawArea.width()) * 0.2;
        } else {
            // Moving left - increase value (opposite behavior)
            valueChange = -static_cast<qreal>(deltaX) / static_cast<qreal>(drawArea.width()) * 0.2;
        }
    }
    
    // Calculate new value
    qreal newValue = m_currentValue + valueChange;
    
    // Clamp value between 0.0 and 1.0
    newValue = qBound(0.0, newValue, 1.0);
    
    qDebug() << "Value change:" << valueChange << "New value:" << newValue;
    
    // Update if value changed
    if (qAbs(newValue - m_currentValue) > 0.001) { // Small threshold to avoid constant updates
        qDebug() << "Updating indicator value to:" << newValue;
        setIndicatorValue(newValue);
    }
}
