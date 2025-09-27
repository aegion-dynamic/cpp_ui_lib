#ifndef GRAPHCONTAINER_H
#define GRAPHCONTAINER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDateTime>
#include <vector>
#include <map>
#include <QString>
#include "waterfallgraph.h"
#include "waterfalldata.h"
#include "timeselectionvisualizer.h"
#include "timelineview.h"
#include "zoompanel.h"
#include "timelineutils.h"
#include "graphtype.h"
#include "customwaterfallgraph.h"
#include "fdwgraph.h"
#include "bdwgraph.h"
#include "brwgraph.h"
#include "ltwgraph.h"
#include "btwgraph.h"
#include "rtwgraph.h"
#include "ftwgraph.h"

class GraphContainer : public QWidget
{
    Q_OBJECT
public:
    explicit GraphContainer(QWidget *parent = nullptr, bool showTimelineView = true);
    void setShowTimelineView(bool showTimelineView);
    bool getShowTimelineView();
    
    // Sizing methods
    int getTimelineWidth() const;
    
    // Graph view sizing methods
    void setGraphViewSize(int width, int height);
    QSize getGraphViewSize() const;
    QSize getTotalContainerSize() const;
    
    // Data point methods
    void setData(const std::vector<qreal>& yData, const std::vector<QDateTime>& timestamps);
    void setData(const WaterfallData& data);
    void clearData();
    void addDataPoint(qreal yValue, const QDateTime& timestamp);
    void addDataPoints(const std::vector<qreal>& yValues, const std::vector<QDateTime>& timestamps);
    
    // Data access methods
    WaterfallData getData() const;
    std::vector<std::pair<qreal, QDateTime>> getDataWithinYExtents(qreal yMin, qreal yMax) const;
    std::vector<std::pair<qreal, QDateTime>> getDataWithinTimeRange(const QDateTime& startTime, const QDateTime& endTime) const;
    
    // Min/max data methods
    qreal getMinY() const;
    qreal getMaxY() const;
    std::pair<qreal, qreal> getYRange() const;
    
    // Data options management
    void addDataOption(const QString& title, WaterfallData& dataSource);
    void removeDataOption(const QString& title);
    void clearDataOptions();
    void setCurrentDataOption(const QString& title);
    QString getCurrentDataOption() const;
    std::vector<QString> getAvailableDataOptions() const;
    WaterfallData* getDataOption(const QString& title);
    bool hasDataOption(const QString& title) const;
    
    // Signal subscription method for external components
    void subscribeToIntervalChange(QObject* subscriber, const char* slot);
    
    // Mouse selection control
    void setMouseSelectionEnabled(bool enabled);
    bool isMouseSelectionEnabled() const;

    // Set the current time 
    void setCurrentTime(const QTime& time);

    
    // Test method
    void testSelectionRectangle();
    
    // Public method for external components to update zoom panel limits
    void initializeZoomPanelLimits();
    
public slots:
    void onTimeIntervalChanged(TimeInterval interval);
    void onSelectionCreated(const TimeSelectionSpan& selection);
    void onZoomValueChanged(ZoomBounds bounds);
    
private:
    void updateTotalContainerSize();
    void updateComboBoxOptions();
    void onDataOptionChanged(int index);
    void setupEventConnections();
    WaterfallGraph* createWaterfallGraph(GraphType graphType);
    
    // Data source management
    WaterfallData waterfallData;
    
    // Data options management
    std::map<QString, WaterfallData*> dataOptions;
    QString currentDataOption;


    // Graph component management
    std::map<QString, WaterfallGraph*> waterfallGraphs;
    QString currentWaterfallGraph;

signals:
    void NewTimeSelectionCreated(qreal startTime, qreal endTime);
    void DeltaTimeSelectionChanged(qreal deltaTime);
    void TimeSelectionsCleared();
    void IntervalChanged(TimeInterval interval);

private:
    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_leftLayout;
    QComboBox *m_comboBox;
    ZoomPanel *m_zoomPanel;
    WaterfallGraph *m_waterfallGraph;
    TimeSelectionVisualizer *m_timelineSelectionView;
    TimelineView *m_timelineView;
    bool m_showTimelineView;
    
    // Sizing properties
    int m_timelineWidth;
    QSize m_graphViewSize;
};

#endif // GRAPHCONTAINER_H
