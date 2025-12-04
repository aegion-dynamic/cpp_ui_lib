#ifndef BTWGRAPH_H
#define BTWGRAPH_H

#include "waterfallgraph.h"
#include "btwsymboldrawing.h"
#include "waterfalldata.h"  // For BTWSymbolData
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDateTime>
#include <vector>

// Forward declarations to avoid circular dependency
class InteractiveGraphicsItem;

/**
 * @brief BTW Graph component that inherits from waterfallgraph
 *
 * This component creates scatterplots by default and can be extended
 * for specific BTW (Bit Time Waterfall) functionality.
 * Interactive markers are managed by WaterfallGraph's unified marker system.
 */
class BTWGraph : public WaterfallGraph
{
    Q_OBJECT

public:
    explicit BTWGraph(QWidget *parent = nullptr, bool enableGrid = false, int gridDivisions = 10, TimeInterval timeInterval = TimeInterval::FifteenMinutes);
    ~BTWGraph();

    /**
     * @brief Get all timestamps from automatic markers
     * @return Vector of timestamps from all automatic markers that were created
     */
    std::vector<QDateTime> getAutomaticMarkerTimestamps() const;
    
    /**
     * @brief Add a BTW symbol to the graph
     * @param symbolName Name of the symbol (e.g., "MagentaCircle")
     * @param timestamp Timestamp when the symbol should be displayed
     * @param range Range value (Y-axis position) where the symbol should be displayed
     */
    void addBTWSymbol(const QString &symbolName, const QDateTime &timestamp, qreal range);

public slots:
    void deleteInteractiveMarkers();

protected:
    // Override the draw method to create scatterplots by default
    void draw() override;

    // Override mouse event handlers to add interactive markers on click
    void onMouseClick(const QPointF &scenePos) override;
    void onMouseDrag(const QPointF &scenePos) override;

private:
    // BTW-specific properties and methods can be added here
    void drawBTWScatterplot();
    void drawCustomCircleMarkers(); // DEPRECATED - kept for backward compatibility
    void drawBTWSymbols();
    void syncMarkersFromDataSource(); // Sync markers from sync state to unified marker system
    BTWSymbolDrawing::SymbolType symbolNameToType(const QString &symbolName) const;
    void addBTWSymbolToOtherGraphs(const QDateTime &timestamp, qreal btwValue);
    
    // BTW symbol drawing utility (symbols are stored in WaterfallData)
    BTWSymbolDrawing symbols;
    
    // Store timestamps from automatic markers
    std::vector<QDateTime> m_automaticMarkerTimestamps;

signals:
    /**
     * @brief Emitted when a manual marker is placed
     * @param timestamp The timestamp of the placed marker
     * @param position The scene position where the marker was placed
     */
    void manualMarkerPlaced(const QDateTime &timestamp, const QPointF &position);
    
    /**
     * @brief Emitted when a manual marker is clicked
     * @param timestamp The timestamp of the clicked marker
     * @param position The scene position where the marker was clicked
     */
    void manualMarkerClicked(const QDateTime &timestamp, const QPointF &position);
};

#endif // BTWGRAPH_H
