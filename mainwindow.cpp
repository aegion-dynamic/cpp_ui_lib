#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), timer(new QTimer(this)), timeUpdateTimer(new QTimer(this))
{
    ui->setupUi(this);

    // Create GraphLayout programmatically
    std::vector<QString> dataSourceLabels = {"LTW", "BTW", "RTW"};
    graphgrid = new GraphLayout(ui->originalTab, LayoutType::GPW4W, dataSourceLabels);
    graphgrid->setObjectName("graphgrid");
    graphgrid->setGeometry(QRect(970, 70, 611, 651));

    

    // inside MainWindow constructor
    std::srand(std::time(nullptr));

    this->simTick = 0;
    // Set up timer for simulation updates (every 2 seconds)
    connect(timer, &QTimer::timeout, this, &MainWindow::updateSimulation);
    timer->start(2000); // 2000ms = 2 seconds
    

    // Initialize some sample data for the graph
    std::vector<double> x_data = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<double> y1_data = {0.0, 2.0, 4.0, 6.0, 8.0};  // Linear growth
    std::vector<double> y2_data = {0.0, 1.0, 4.0, 9.0, 16.0}; // Quadratic growth

    // Set the data to the graph widget
    ui->widget->setData(x_data, y1_data, y2_data);
    ui->widget->setAxesLabels("Time (s)", "Speed (m/s)", "Distance (m)");

    // Our ship
    this->currentShipSpeed = 30;
    this->currentOwnShipBearing = 90; //  Nautical degrees

    this->currentSensorBearing = 250;

    // selected track
    this->currentSelectedTrackSpeed = 30;
    this->currentSelectedTrackRange = 9;
    this->currentSelectedTrackBearing = 200;
    this->currentSelectedTrackCourse = 180;

    // adopted track
    this->currentAdoptedTrackSpeed = 30;
    this->currentAdoptedTrackRange = 10;
    this->currentAdoptedTrackBearing = 300;
    this->currentAdoptedTrackCourse = 270;

    ui->widget_2->setData(
        this->currentShipSpeed,
        this->currentOwnShipBearing,
        this->currentSensorBearing,
        this->currentAdoptedTrackRange,
        this->currentAdoptedTrackSpeed,
        this->currentAdoptedTrackBearing,
        this->currentSelectedTrackRange,
        this->currentSelectedTrackSpeed,
        this->currentSelectedTrackBearing,
        this->currentAdoptedTrackCourse,
        this->currentSelectedTrackCourse);

    // Configure layout selection combobox
    configureLayoutSelection();
    
    // Demonstrate data point methods
    demonstrateDataPointMethods();
    
    // Setup custom graphs tab
    setupCustomGraphsTab();

    // // Configure TimeSelectionVisualizer 
    // configureTimeVisualizer();

    // // Configure TimelineView
    // configureTimelineView();
    
    // // Configure Zoom Panel test functionality
    // configureZoomPanel();

    // // Set up timer for current time updates (every second)
    // connect(timeUpdateTimer, &QTimer::timeout, this, &MainWindow::updateCurrentTime);
    // timeUpdateTimer->start(1000); // 1000ms = 1 second
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Simulation update slot - called every timer interval
 *
 * Updates target position, recalculates bearing/range/rate,
 * and triggers widget repaint. This is the main simulation loop.
 */
void MainWindow::updateSimulation()
{
    this->simTick++;
    // qDebug() << "Sim Tick: " << this->simTick;

    auto randPercent = [](int max)
    {
        return static_cast<double>(std::rand() % max) / 100.0; // 0.00 .. (max-1)/100
    };

    // --- Own ship (small variations <10%) ---
    this->currentShipSpeed *= (0.95 + randPercent(10));   // ±5%
    this->currentOwnShipBearing += (std::rand() % 7) - 3; // jitter ±3°

    // --- Adopted track (small variations <10%) ---
    this->currentAdoptedTrackSpeed *= (0.95 + randPercent(10)); // ±5%
    this->currentAdoptedTrackRange *= (0.95 + randPercent(10)); // ±5%
    this->currentAdoptedTrackBearing += (std::rand() % 9) - 4;  // jitter ±4°
    this->currentAdoptedTrackCourse += (std::rand() % 7) - 3;  // jitter ±3°

    // --- Selected track (large variations >30%) ---
    this->currentSelectedTrackSpeed *= (0.7 + randPercent(60));   // 0.7–1.29 → ±30%
    this->currentSelectedTrackRange *= (0.7 + randPercent(60));   // ±30%
    this->currentSelectedTrackBearing += (std::rand() % 91) - 45; // swing ±45°
    this->currentSelectedTrackCourse += (std::rand() % 91) - 45; // swing ±45°

    // Normalize bearings and courses to [0, 360)
    this->currentOwnShipBearing = fmod(this->currentOwnShipBearing + 360.0, 360.0);
    this->currentAdoptedTrackBearing = fmod(this->currentAdoptedTrackBearing + 360.0, 360.0);
    this->currentSelectedTrackBearing = fmod(this->currentSelectedTrackBearing + 360.0, 360.0);
    this->currentAdoptedTrackCourse = fmod(this->currentAdoptedTrackCourse + 360.0, 360.0);
    this->currentSelectedTrackCourse = fmod(this->currentSelectedTrackCourse + 360.0, 360.0);

    // Push updated values into TacticalSolutionView
    ui->widget_2->setData(
        this->currentShipSpeed,
        this->currentOwnShipBearing,
        this->currentSensorBearing,
        this->currentAdoptedTrackRange,
        this->currentAdoptedTrackSpeed,
        this->currentAdoptedTrackBearing,
        this->currentSelectedTrackRange,
        this->currentSelectedTrackSpeed,
        this->currentSelectedTrackBearing,
        this->currentAdoptedTrackCourse,
        this->currentSelectedTrackCourse);


    // set the current time to the system time
    graphgrid->setCurrentTime(QTime::currentTime());
}

// void MainWindow::configureTimeVisualizer()
// {
//     // Set timeline length to 1 hour (60 minutes)
//     ui->timeVisualizer->setTimeLineLength(QTime(1, 0, 0));
    
//     // Set initial current time to system time
//     ui->timeVisualizer->setCurrentTime(QTime::currentTime());
    
//     // Add some sample time selections relative to current time
//     QTime currentTime = QTime::currentTime();
    
//     // Create selections that are within the visible range (last hour)
//     // Selection 1: 5-10 minutes ago (should be visible)
//     QTime start1 = currentTime.addSecs(-10 * 60); // 10 minutes ago
//     QTime end1 = currentTime.addSecs(-5 * 60);   // 5 minutes ago
//     TimeSelectionSpan span1(start1, end1);
    
//     // Selection 2: 20-25 minutes ago (should be visible)
//     QTime start2 = currentTime.addSecs(-25 * 60); // 25 minutes ago
//     QTime end2 = currentTime.addSecs(-20 * 60);   // 20 minutes ago
//     TimeSelectionSpan span2(start2, end2);
    
//     // Selection 3: 35-45 minutes ago (should be visible)
//     QTime start3 = currentTime.addSecs(-45 * 60); // 45 minutes ago
//     QTime end3 = currentTime.addSecs(-35 * 60);   // 35 minutes ago
//     TimeSelectionSpan span3(start3, end3);
    
//     // Selection 4: 2-3 minutes ago (should be visible)
//     QTime start4 = currentTime.addSecs(-3 * 60); // 3 minutes ago
//     QTime end4 = currentTime.addSecs(-2 * 60);   // 2 minutes ago
//     TimeSelectionSpan span4(start4, end4);
    
    
//     ui->timeVisualizer->addTimeSelection(span1);
//     ui->timeVisualizer->addTimeSelection(span2);
//     ui->timeVisualizer->addTimeSelection(span3);
//     ui->timeVisualizer->addTimeSelection(span4);
    
// }

// void MainWindow::configureTimelineView()
// {
//     ui->timelineView->setTimeLineLength(TimeInterval::FifteenMinutes); // 15 minutes
//     ui->timelineView->setCurrentTime(QTime::currentTime());
//     ui->timelineView->setNumberOfDivisions(15); // 15 segments

//     // Create a new timer that fires every minute
//     QTimer *timelineUpdateTimer = new QTimer(this);
//     connect(timelineUpdateTimer, &QTimer::timeout, this, &MainWindow::updateTimeline);
//     timelineUpdateTimer->start(60000); // 60000ms = 1 minute
    
// }

// void MainWindow::updateTimeline()
// {
//     ui->timelineView->setCurrentTime(QTime::currentTime());
// }


// void MainWindow::updateCurrentTime()
// {
//     // Update the current time to the system time
//     ui->timeVisualizer->setCurrentTime(QTime::currentTime());
//     ui->timelineView->setCurrentTime(QTime::currentTime());
// }

// void MainWindow::configureZoomPanel()
// {
//     // Connect test buttons to zoom panel indicator
//     connect(ui->testButton0, &QPushButton::clicked, [this]() {
//         ui->zoomPanel->setIndicatorValue(0.0);
//     });
    
//     connect(ui->testButton50, &QPushButton::clicked, [this]() {
//         ui->zoomPanel->setIndicatorValue(0.5);
//     });
    
//     connect(ui->testButton100, &QPushButton::clicked, [this]() {
//         ui->zoomPanel->setIndicatorValue(1.0);
//     });
        
//     // Initialize zoom panel with a default value
//     ui->zoomPanel->setIndicatorValue(0.3);
    
//     // Initialize label values
//     ui->zoomPanel->setLeftLabelValue(0.0);  // Left reference value
//     ui->zoomPanel->setCenterLabelValue(0.5); // Center value
//     ui->zoomPanel->setRightLabelValue(1.0);  // Range for upper bound
    
//     // Initialize the indicator value label
//     ui->indicatorValueLabel->setText("Bounds: [0.35, 0.80]");
    
//     // Connect to zoom panel value changed signal
//     connect(ui->zoomPanel, &ZoomPanel::valueChanged, [this](ZoomBounds bounds) {
//         // Update the indicator value label on main window with bounds
//         ui->indicatorValueLabel->setText(QString("Bounds: [%1, %2]").arg(bounds.lowerbound, 0, 'f', 2).arg(bounds.upperbound, 0, 'f', 2));
        
//         // You can add additional logic here to respond to value changes
//         // For example, update other UI elements or trigger other actions
//     });
// }

void MainWindow::configureLayoutSelection()
{
    // Populate combobox with layout type options
    ui->layoutSelectionComboBox->addItem("1 Window", static_cast<int>(LayoutType::GPW1W));
    ui->layoutSelectionComboBox->addItem("4 Windows (2x2)", static_cast<int>(LayoutType::GPW4W));
    ui->layoutSelectionComboBox->addItem("2 Windows Vertical", static_cast<int>(LayoutType::GPW2WV));
    ui->layoutSelectionComboBox->addItem("2 Windows Horizontal", static_cast<int>(LayoutType::GPW2WH));
    ui->layoutSelectionComboBox->addItem("4 Windows Horizontal", static_cast<int>(LayoutType::GPW4WH));
    ui->layoutSelectionComboBox->addItem("Hidden", static_cast<int>(LayoutType::HIDDEN));
    
    // Set default selection to 1 Window
    ui->layoutSelectionComboBox->setCurrentIndex(5);
    
    // Connect combobox to slot
    connect(ui->layoutSelectionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLayoutTypeChanged);
}

void MainWindow::onLayoutTypeChanged(int index)
{
    LayoutType layoutType = static_cast<LayoutType>(ui->layoutSelectionComboBox->itemData(index).toInt());
    graphgrid->setLayoutType(layoutType);
}

void MainWindow::demonstrateDataPointMethods()
{
    qDebug() << "=== Data Source Demonstration ===";
    
    // Demonstrate the new data source methods with 15-minute historical data
    QDateTime currentTime = QDateTime::currentDateTime();
    
    // Create realistic mock data spanning the last 15 minutes
    // LTW (Left Track Window) - Simulates radar contact data
    std::vector<qreal> ltwYData;
    std::vector<QDateTime> ltwTimestamps;
    for (int i = 0; i < 15; ++i) {
        // Data points every minute going backwards from current time
        QDateTime timestamp = currentTime.addMSecs(-i * 60 * 1000); // i minutes ago
        ltwTimestamps.push_back(timestamp);
        
        // Simulate realistic radar range values (5-25 nautical miles)
        qreal baseRange = 15.0 + (i % 3) * 3.0; // Vary between 15-21 nm
        qreal noise = (std::rand() % 200 - 100) / 100.0; // ±1 nm noise
        ltwYData.push_back(baseRange + noise);
    }
    graphgrid->setDataToDataSource("LTW", ltwYData, ltwTimestamps);
    
    // BTW (Bottom Track Window) - Simulates sonar contact data
    std::vector<qreal> btwYData;
    std::vector<QDateTime> btwTimestamps;
    for (int i = 0; i < 12; ++i) {
        // Data points every 1.25 minutes going backwards
        QDateTime timestamp = currentTime.addMSecs(-i * 75 * 1000); // i * 1.25 minutes ago
        btwTimestamps.push_back(timestamp);
        
        // Simulate realistic sonar depth values (50-200 meters)
        qreal baseDepth = 120.0 + (i % 4) * 20.0; // Vary between 120-180m
        qreal noise = (std::rand() % 100 - 50) / 10.0; // ±5m noise
        btwYData.push_back(baseDepth + noise);
    }
    graphgrid->setDataToDataSource("BTW", btwYData, btwTimestamps);
    
    // RTW (Right Track Window) - Simulates bearing data
    std::vector<qreal> rtwYData;
    std::vector<QDateTime> rtwTimestamps;
    for (int i = 0; i < 18; ++i) {
        // Data points every 50 seconds going backwards
        QDateTime timestamp = currentTime.addMSecs(-i * 50 * 1000); // i * 50 seconds ago
        rtwTimestamps.push_back(timestamp);
        
        // Simulate realistic bearing values (0-360 degrees)
        qreal baseBearing = 180.0 + (i % 6) * 30.0; // Vary between 180-330 degrees
        qreal noise = (std::rand() % 20 - 10); // ±10 degree noise
        qreal bearing = fmod(baseBearing + noise + 360.0, 360.0);
        rtwYData.push_back(bearing);
    }
    graphgrid->setDataToDataSource("RTW", rtwYData, rtwTimestamps);
    
    // Add some additional older data points to simulate historical data (not out of order)
    graphgrid->addDataPointToDataSource("LTW", 18.5, currentTime.addMSecs(-16 * 60 * 1000)); // 16 minutes ago
    graphgrid->addDataPointToDataSource("BTW", 135.2, currentTime.addMSecs(-16 * 60 * 1000)); // 16 minutes ago  
    graphgrid->addDataPointToDataSource("RTW", 195.7, currentTime.addMSecs(-16 * 60 * 1000)); // 16 minutes ago
    
    // Demonstrate NEW label-based container APIs
    qDebug() << "=== NEW Label-based Container API Demonstration ===";
    
    // Test container management
    qDebug() << "Available container labels:" << graphgrid->getContainerLabels().size();
    qDebug() << "Has LTW container:" << graphgrid->hasContainer("LTW");
    qDebug() << "Has INVALID container:" << graphgrid->hasContainer("INVALID");
    
    // Test label-based data option methods
    qDebug() << "Current data option for LTW:" << graphgrid->getCurrentDataOption("LTW");
    qDebug() << "Available data options for LTW:" << graphgrid->getAvailableDataOptions("LTW").size();
    
    // Demonstrate data source management
    qDebug() << "Available data sources:" << graphgrid->getDataSourceLabels().size();
    qDebug() << "Has LTW data source:" << graphgrid->hasDataSource("LTW");
    qDebug() << "Has INVALID data source:" << graphgrid->hasDataSource("INVALID");
    
    // Get data sources and check their data
    WaterfallData* ltwData = graphgrid->getDataSource("LTW");
    if (ltwData) {
        qDebug() << "LTW data size:" << ltwData->getDataSize();
        qDebug() << "LTW Y range:" << ltwData->getMinY() << "to" << ltwData->getMaxY();
        qDebug() << "LTW time span:" << ltwData->getTimeSpanMs() / 1000.0 << "seconds";
    }
    
    WaterfallData* btwData = graphgrid->getDataSource("BTW");
    if (btwData) {
        qDebug() << "BTW data size:" << btwData->getDataSize();
        qDebug() << "BTW Y range:" << btwData->getMinY() << "to" << btwData->getMaxY();
    }
    
    WaterfallData* rtwData = graphgrid->getDataSource("RTW");
    if (rtwData) {
        qDebug() << "RTW data size:" << rtwData->getDataSize();
        qDebug() << "RTW Y range:" << rtwData->getMinY() << "to" << rtwData->getMaxY();
    }
    
    qDebug() << "Data source methods demonstrated successfully with 15-minute historical data";
    qDebug() << "NEW label-based container APIs demonstrated successfully";
}

void MainWindow::setupCustomGraphsTab()
{
    qDebug() << "=== Setting up Custom Graphs Tab ===";
    
    // Create a grid layout for the custom graphs tab
    QGridLayout *gridLayout = new QGridLayout(ui->customGraphsTab);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(10, 10, 10, 10);
    
    // Create 4 custom waterfall graphs with different styles and colors
    
    // Graph 1: Line chart style with blue theme
    customGraph1 = new CustomWaterfallGraph(ui->customGraphsTab, true, 8, TimeInterval::FifteenMinutes);
    customGraph1->setObjectName("customGraph1");
    customGraph1->setMouseSelectionEnabled(true);
    customGraph1->setDrawingStyle("default");
    customGraph1->setCustomColors(
        QColor(100, 150, 255),  // Blue data color
        QColor(200, 200, 200), // Light gray grid
        QColor(20, 30, 50)      // Dark blue background
    );
    
    // Graph 2: Area chart style with green theme
    customGraph2 = new CustomWaterfallGraph(ui->customGraphsTab, true, 6, TimeInterval::FifteenMinutes);
    customGraph2->setObjectName("customGraph2");
    customGraph2->setMouseSelectionEnabled(true);
    customGraph2->setDrawingStyle("area");
    customGraph2->setCustomColors(
        QColor(100, 255, 150),  // Green data color
        QColor(180, 180, 180), // Light gray grid
        QColor(20, 50, 30)      // Dark green background
    );
    
    // Graph 3: Points style with red theme
    customGraph3 = new CustomWaterfallGraph(ui->customGraphsTab, true, 10, TimeInterval::FifteenMinutes);
    customGraph3->setObjectName("customGraph3");
    customGraph3->setMouseSelectionEnabled(true);
    customGraph3->setDrawingStyle("points");
    customGraph3->setCustomColors(
        QColor(255, 100, 100),  // Red data color
        QColor(220, 220, 220), // Light gray grid
        QColor(50, 20, 20)      // Dark red background
    );
    
    // Graph 4: Default style with purple theme
    customGraph4 = new CustomWaterfallGraph(ui->customGraphsTab, true, 12, TimeInterval::FifteenMinutes);
    customGraph4->setObjectName("customGraph4");
    customGraph4->setMouseSelectionEnabled(true);
    customGraph4->setDrawingStyle("default");
    customGraph4->setCustomColors(
        QColor(200, 100, 255),  // Purple data color
        QColor(200, 200, 200), // Light gray grid
        QColor(40, 20, 50)      // Dark purple background
    );
    
    // Add graphs to grid layout (2x2)
    gridLayout->addWidget(customGraph1, 0, 0);
    gridLayout->addWidget(customGraph2, 0, 1);
    gridLayout->addWidget(customGraph3, 1, 0);
    gridLayout->addWidget(customGraph4, 1, 1);
    
    // Set equal column and row stretches
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setRowStretch(0, 1);
    gridLayout->setRowStretch(1, 1);
    
    // Create sample data for each graph
    setupCustomGraphData();
    
    qDebug() << "Custom graphs tab setup completed successfully";
}

void MainWindow::setupCustomGraphData()
{
    qDebug() << "=== Setting up Custom Graph Data ===";
    
    QDateTime now = QDateTime::currentDateTime();
    
    // Graph 1 Data: Bar chart with ascending values
    WaterfallData* graph1Data = new WaterfallData("Bar Chart Data");
    std::vector<qreal> graph1Values = {5.0, 8.0, 12.0, 15.0, 18.0, 22.0, 25.0, 28.0, 30.0, 32.0};
    std::vector<QDateTime> graph1Timestamps;
    for (int i = 0; i < graph1Values.size(); ++i) {
        graph1Timestamps.push_back(now.addSecs(-i * 60));
    }
    graph1Data->setData(graph1Values, graph1Timestamps);
    customGraph1->setDataSource(*graph1Data);
    
    // Graph 2 Data: Area chart with wave pattern
    WaterfallData* graph2Data = new WaterfallData("Area Chart Data");
    std::vector<qreal> graph2Values = {10.0, 15.0, 20.0, 18.0, 12.0, 8.0, 14.0, 22.0, 16.0, 11.0};
    std::vector<QDateTime> graph2Timestamps;
    for (int i = 0; i < graph2Values.size(); ++i) {
        graph2Timestamps.push_back(now.addSecs(-i * 60));
    }
    graph2Data->setData(graph2Values, graph2Timestamps);
    customGraph2->setDataSource(*graph2Data);
    
    // Graph 3 Data: Points with random pattern
    WaterfallData* graph3Data = new WaterfallData("Points Data");
    std::vector<qreal> graph3Values = {8.0, 25.0, 12.0, 30.0, 15.0, 22.0, 18.0, 28.0, 20.0, 16.0};
    std::vector<QDateTime> graph3Timestamps;
    for (int i = 0; i < graph3Values.size(); ++i) {
        graph3Timestamps.push_back(now.addSecs(-i * 60));
    }
    graph3Data->setData(graph3Values, graph3Timestamps);
    customGraph3->setDataSource(*graph3Data);
    
    // Graph 4 Data: Default with mixed pattern
    WaterfallData* graph4Data = new WaterfallData("Default Data");
    std::vector<qreal> graph4Values = {15.0, 20.0, 18.0, 25.0, 22.0, 28.0, 24.0, 30.0, 26.0, 23.0};
    std::vector<QDateTime> graph4Timestamps;
    for (int i = 0; i < graph4Values.size(); ++i) {
        graph4Timestamps.push_back(now.addSecs(-i * 60));
    }
    graph4Data->setData(graph4Values, graph4Timestamps);
    customGraph4->setDataSource(*graph4Data);
    
    // Add some custom elements to each graph
    customGraph1->drawCharacterLabel("Line Chart", QPointF(10, 10), QColor(255, 255, 255), 14);
    customGraph1->drawPoint(QPointF(50, 50), QColor(255, 255, 0), 6.0);
    
    customGraph2->drawCharacterLabel("Area Chart", QPointF(10, 10), QColor(255, 255, 255), 14);
    customGraph2->drawTriangleMarker(QPointF(100, 80), QColor(255, 255, 0), QColor(255, 255, 255), 10.0);
    
    customGraph3->drawCharacterLabel("Points Chart", QPointF(10, 10), QColor(255, 255, 255), 14);
    customGraph3->drawAxisLine(QPointF(20, 20), QPointF(180, 20), QColor(255, 255, 255, 128));
    
    customGraph4->drawCharacterLabel("Default Chart", QPointF(10, 10), QColor(255, 255, 255), 14);
    customGraph4->drawScatterplot(QColor(255, 255, 0), 4.0, QColor(255, 255, 255));
    
    qDebug() << "Custom graph data setup completed successfully";
}

