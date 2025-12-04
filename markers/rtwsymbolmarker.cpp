#include "rtwsymbolmarker.h"
#include "../waterfallgraph.h"
#include "../rtwsymboldrawing.h"
#include <QGraphicsPixmapItem>
#include <QDebug>

// RTWSymbolMarker implementation
RTWSymbolMarker::RTWSymbolMarker(const QString &symbolName, const QDateTime &timestamp, qreal range)
    : NonInteractiveWaterfallMarker(MarkerType::RTWSymbolMarker, timestamp, range)
    , m_symbolName(symbolName)
{
    // Update hash to include symbol name
    qint64 timestampSecs = timestamp.toSecsSinceEpoch();
    uint hashValue = qHash(static_cast<int>(m_type)) ^ qHash(m_symbolName) ^ qHash(m_value) ^ qHash(timestampSecs);
    m_hash = QString::number(hashValue, 16);
}

void RTWSymbolMarker::draw(QGraphicsScene *scene, WaterfallGraph *graph)
{
    if (!scene || !graph) {
        return;
    }

    // Map data coordinates to screen coordinates
    QPointF screenPos = graph->mapDataToScreenPublic(m_value, m_timestamp);
    
    // Check if point is within visible area
    QRectF drawingArea = graph->getDrawingArea();
    if (!drawingArea.contains(screenPos)) {
        return;
    }

    // Convert symbol name to SymbolType
    RTWSymbolDrawing::SymbolType symbolType;
    QString name = m_symbolName.toUpper().trimmed();
    
    if (name == "TM") symbolType = RTWSymbolDrawing::SymbolType::TM;
    else if (name == "DP") symbolType = RTWSymbolDrawing::SymbolType::DP;
    else if (name == "LY") symbolType = RTWSymbolDrawing::SymbolType::LY;
    else if (name == "CIRCLEI" || name == "CIRCLE_I") symbolType = RTWSymbolDrawing::SymbolType::CircleI;
    else if (name == "TRIANGLE") symbolType = RTWSymbolDrawing::SymbolType::Triangle;
    else if (name == "RECTR" || name == "RECT_R") symbolType = RTWSymbolDrawing::SymbolType::RectR;
    else if (name == "ELLIPSEPP" || name == "ELLIPSE_PP") symbolType = RTWSymbolDrawing::SymbolType::EllipsePP;
    else if (name == "RECTX" || name == "RECT_X") symbolType = RTWSymbolDrawing::SymbolType::RectX;
    else if (name == "RECTA" || name == "RECT_A") symbolType = RTWSymbolDrawing::SymbolType::RectA;
    else if (name == "RECTAPURPLE" || name == "RECT_A_PURPLE") symbolType = RTWSymbolDrawing::SymbolType::RectAPurple;
    else if (name == "RECTK" || name == "RECT_K") symbolType = RTWSymbolDrawing::SymbolType::RectK;
    else if (name == "CIRCLERYELLOW" || name == "CIRCLE_R_YELLOW") symbolType = RTWSymbolDrawing::SymbolType::CircleRYellow;
    else if (name == "DOUBLEBARYELLOW" || name == "DOUBLE_BAR_YELLOW") symbolType = RTWSymbolDrawing::SymbolType::DoubleBarYellow;
    else if (name == "R") symbolType = RTWSymbolDrawing::SymbolType::R;
    else if (name == "L") symbolType = RTWSymbolDrawing::SymbolType::L;
    else if (name == "BOT") symbolType = RTWSymbolDrawing::SymbolType::BOT;
    else if (name == "BOTC") symbolType = RTWSymbolDrawing::SymbolType::BOTC;
    else if (name == "BOTF") symbolType = RTWSymbolDrawing::SymbolType::BOTF;
    else if (name == "BOTD") symbolType = RTWSymbolDrawing::SymbolType::BOTD;
    else {
        qDebug() << "RTW: Unknown symbol name:" << m_symbolName << "- defaulting to R";
        symbolType = RTWSymbolDrawing::SymbolType::R;
    }

    // Get the pixmap for this symbol type
    static RTWSymbolDrawing symbols(40); // Static instance to cache pixmaps
    const QPixmap& symbolPixmap = symbols.get(symbolType);
    
    // Validate pixmap before using it
    if (symbolPixmap.isNull() || symbolPixmap.width() <= 0 || symbolPixmap.height() <= 0) {
        qDebug() << "RTW: Invalid pixmap for symbol" << m_symbolName << "type" << static_cast<int>(symbolType) << "- skipping";
        return;
    }
    
    // Create a graphics pixmap item and add it to the scene
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(symbolPixmap);
    
    // Validate pixmap item was created successfully
    if (!pixmapItem) {
        qDebug() << "RTW: Failed to create pixmap item for symbol" << m_symbolName << "- skipping";
        return;
    }
    
    // Center the symbol on the data point
    QRectF pixmapRect = pixmapItem->boundingRect();
    if (pixmapRect.width() <= 0 || pixmapRect.height() <= 0) {
        qDebug() << "RTW: Invalid pixmap rect for symbol" << m_symbolName << "- skipping";
        delete pixmapItem;
        return;
    }
    
    pixmapItem->setPos(screenPos.x() - pixmapRect.width() / 2, 
                      screenPos.y() - pixmapRect.height() / 2);
    pixmapItem->setZValue(1000); // High z-value to ensure visibility above other elements
    
    scene->addItem(pixmapItem);
}

