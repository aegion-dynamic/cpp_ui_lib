#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <cstdlib>
#include <ctime>
#include "timeselectionvisualizer.h"
#include "timelineview.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QTimer *timer; ///< Timer for simulation updates
    QTimer *timeUpdateTimer; ///< Timer for updating current time
    
    void configureTimeVisualizer();

    long simTick;

    // Sensor Bearing
    qreal currentSensorBearing;
    qreal prevSensorBearing;

    // Own Ship Info
    qreal currentOwnShipBearing;
    qreal currentShipSpeed;
    qreal prevOwnShipBearing;
    qreal prevShipSpeed;

    // Selected Track Info
    qreal currentSelectedTrackRange;
    qreal currentSelectedTrackBearing;
    qreal currentSelectedTrackSpeed;
    qreal currentSelectedTrackCourse;

    qreal prevSelectedTrackRange;
    qreal prevSelectedTrackBearing;
    qreal prevSelectedTrackSpeed;
    qreal prevSelectedTrackCourse;

    // Adopted Track Info
    qreal currentAdoptedTrackRange;
    qreal currentAdoptedTrackBearing;
    qreal currentAdoptedTrackSpeed;
    qreal currentAdoptedTrackCourse;

    qreal prevAdoptedTrackRange;
    qreal prevAdoptedTrackBearing;
    qreal prevAdoptedTrackSpeed;
    qreal prevAdoptedTrackCourse;

private slots:
    /**
     * @brief Updates simulation state every timer interval
     *
     * Called every 2 seconds to update target position, bearing, range,
     * and bearing rate calculations. Triggers widget repaint.
     */
    void updateSimulation();
    
    /**
     * @brief Updates the current time in the time visualizer
     *
     * Called every second to update the current time to system time.
     */
    void updateCurrentTime();
    
};

#endif // MAINWINDOW_H
