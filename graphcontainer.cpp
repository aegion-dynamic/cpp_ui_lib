#include "graphcontainer.h"

GraphContainer::GraphContainer(QWidget *parent, bool showTimelineView)
    : QWidget{parent}, m_showTimelineView(showTimelineView)
{
    // Create main horizontal layout
    m_mainLayout = new QHBoxLayout(this);
    
    // Create left vertical layout
    m_leftLayout = new QVBoxLayout();
    
    // Create ComboBox
    m_comboBox = new QComboBox(this);
    m_comboBox->addItem("Option 1");
    m_comboBox->addItem("Option 2");
    m_comboBox->addItem("Option 3");
    
    // Create WaterfallGraph
    m_waterfallGraph = new waterfallgraph(this);
    
    // Add ComboBox and WaterfallGraph to left layout
    m_leftLayout->addWidget(m_comboBox);
    m_leftLayout->addWidget(m_waterfallGraph);
    
    // Add left layout to main layout
    m_mainLayout->addLayout(m_leftLayout);
    
    // Create TimelineSelectionView
    m_timelineSelectionView = new TimeSelectionVisualizer(this);
    m_mainLayout->addWidget(m_timelineSelectionView);
    
    // Create TimelineView (conditionally based on showTimelineView)
    if (m_showTimelineView) {
        m_timelineView = new TimelineView(this);
        m_mainLayout->addWidget(m_timelineView);
    } else {
        m_timelineView = nullptr;
    }
    
    // Set layout
    setLayout(m_mainLayout);
}
