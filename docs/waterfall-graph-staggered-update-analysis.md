# Waterfall Graph Staggered Update Issue - Root Cause Analysis

## Problem Description
When `setData()` and `addDataPoint()` are called alternatively at different cadences, the waterfall graph is not updating immediately but rather causing a staggered update effect.

## Root Cause Analysis

### The Problem

1. **`setData()` behavior** (lines 312-330 in waterfallgraph.cpp):
   - Sets `dataRangesValid = false`
   - Calls `draw()` which sets `RenderState::FULL_REDRAW` and immediately calls `drawIncremental()`
   - `FULL_REDRAW` mode clears the entire scene and redraws everything

2. **`addDataPoint()` behavior** (lines 382-401 in waterfallgraph.cpp):
   - Sets `dataRangesValid = false`
   - Calls `markSeriesDirty()` which calls `transitionToAppropriateState()`
   - If state is already `FULL_REDRAW`, `transitionToAppropriateState()` returns early (line 758-760) without changing state
   - Still calls `drawIncremental()` directly

### The Race Condition

When these methods are called alternately at different cadences:

**Scenario 1: `addDataPoint()` called during `setData()` execution**
- `setData()` sets state to `FULL_REDRAW` and starts `drawIncremental()`
- `addDataPoint()` marks series dirty, but state transition is blocked (state is `FULL_REDRAW`)
- `addDataPoint()` calls `drawIncremental()`, which sees `FULL_REDRAW` and clears/redraws everything
- The incremental update from `addDataPoint()` may be lost or cause a second full redraw

**Scenario 2: `setData()` called right after `addDataPoint()`**
- `addDataPoint()` sets state to `INCREMENTAL_UPDATE` and marks series dirty
- `setData()` immediately forces `FULL_REDRAW`, clearing the scene
- The incremental update from `addDataPoint()` is discarded

### Key Issues Identified

1. **`setData()` always forces `FULL_REDRAW`** (line 651), even if an incremental update is pending
2. **`transitionToAppropriateState()` blocks state changes** when `FULL_REDRAW` is active (line 758-760), but `addDataPoint()` still calls `drawIncremental()` anyway
3. **No coordination between the two methods** - they can interfere with each other's state

### Code References

- `setData()`: waterfallgraph.cpp lines 312-330
- `addDataPoint()`: waterfallgraph.cpp lines 382-401
- `draw()`: waterfallgraph.cpp lines 645-655
- `drawIncremental()`: waterfallgraph.cpp lines 661-749
- `transitionToAppropriateState()`: waterfallgraph.cpp lines 755-774
- `setRenderState()`: waterfallgraph.cpp lines 879-896

## Recommended Fix

Make `setData()` respect pending incremental updates:

```cpp
void WaterfallGraph::setData(const QString &seriesLabel, const std::vector<qreal> &yData, const std::vector<QDateTime> &timestamps)
{
    if (!dataSource)
    {
        qDebug() << "Error: No data source set";
        return;
    }

    // Store the data using the data source
    dataSource->setDataSeries(seriesLabel, yData, timestamps);

    qDebug() << "Data set successfully. Size:" << dataSource->getDataSeriesSize(seriesLabel);

    // Mark ranges as invalid so they'll be recalculated
    dataRangesValid = false;

    // Mark this series as dirty instead of forcing full redraw
    // This allows it to work better with incremental updates
    markSeriesDirty(seriesLabel);
    markRangeUpdateNeeded();
    
    // Use incremental draw which will handle the update appropriately
    drawIncremental();
}
```

### Alternative Solutions

1. Add synchronization to prevent concurrent draws
2. Batch updates when both methods are called in quick succession
3. Use a queue/debounce mechanism for rapid updates

## Date
Analysis performed: December 2024

