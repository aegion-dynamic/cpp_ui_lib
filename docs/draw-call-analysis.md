# Draw Call Analysis and Reduction Strategy

## Current Draw Call Patterns

### 1. Missing View Updates After Scene Changes

**Problem**: `drawIncremental()` updates the graphics scene but doesn't call `graphicsView->update()` to trigger a repaint.

**Location**: `waterfallgraph.cpp` lines 659-747

**Impact**: 
- Scene changes may not be visible until another update is triggered
- Relies on Qt's automatic update scheduling which may be delayed
- Inconsistent rendering behavior

**Evidence**:
```cpp
void WaterfallGraph::drawIncremental()
{
    // ... updates graphicsScene ...
    // Missing: graphicsView->update();
}
```

### 2. Redundant Draw Calls

**Problem**: Multiple methods call `draw()` when the state machine would handle updates automatically.

**Locations**:
- `setTimeInterval()` (line ~398): Calls `draw()` after `setTimeInterval()` which already triggers updates
- `updateTimeInterval()` in GraphContainer (line ~882): Calls `draw()` even though `setTimeInterval()` already called it
- `setGridEnabled()` (line ~578): Calls `draw()` immediately
- `setGridDivisions()` (line ~606): Calls `draw()` immediately
- `setTimeRange()`, `setTimeMax()`, `setTimeMin()`: All call `draw()` after setting state

**Impact**:
- Multiple draw calls for single state changes
- Unnecessary CPU usage
- Potential visual flicker

### 3. Immediate Draw Calls vs. Deferred Updates

**Problem**: Many methods call `draw()` or `drawIncremental()` immediately instead of letting the state machine batch updates.

**Locations**:
- `addDataPoint()` (line ~398): Calls `drawIncremental()` immediately
- `addDataPoints()` (line ~425): Calls `drawIncremental()` immediately
- All time range setters: Call `draw()` immediately

**Impact**:
- If multiple data points are added in quick succession, each triggers a draw
- No batching of updates
- Higher CPU usage

### 4. Cursor Layer Update Frequency

**Current**: 60fps timer (16ms interval) that calls `updateCursorLayer()`

**Analysis**: 
- Good: Only calls `cursorView->update()` when `needsUpdate` is true
- Good: Separated from main rendering
- Potential: Could be reduced if cursor position hasn't changed

**Location**: `waterfallgraph.cpp` lines 2776-2838

### 5. GraphContainer Redundant Calls

**Problem**: `GraphContainer::updateTimeInterval()` calls `draw()` on all graphs, then calls `draw()` again on current graph.

**Location**: `graphcontainer.cpp` lines 859-884

**Impact**: Double draw calls when interval changes

## Reduction Strategies

### Strategy 1: Add View Update to drawIncremental()

**Approach**: Add `graphicsView->update()` at the end of `drawIncremental()` only when actual rendering occurred.

**Benefits**:
- Ensures view repaints after scene changes
- Single point of update control
- Can be optimized to only update when needed

### Strategy 2: Remove Redundant Immediate Draw Calls

**Approach**: Remove `draw()` calls from methods that set state, let the state machine handle it automatically.

**Methods to modify**:
- `setTimeInterval()`: Remove `draw()` call (state machine handles it)
- `setGridEnabled()`: Remove `draw()` call, mark for redraw via state
- `setGridDivisions()`: Remove `draw()` call, mark for redraw via state
- `setTimeRange()`, `setTimeMax()`, `setTimeMin()`: Remove `draw()` calls, state machine handles it

**Benefits**:
- Eliminates redundant draw calls
- State machine batches updates
- Cleaner separation of concerns

### Strategy 3: Defer Draw Calls for Data Additions

**Approach**: Instead of calling `drawIncremental()` immediately in `addDataPoint()` and `addDataPoints()`, mark state and defer drawing.

**Options**:
- Option A: Use QTimer::singleShot(0) to defer to next event loop
- Option B: Add a flag and draw once at end of event processing
- Option C: Let caller decide when to draw (remove automatic draws)

**Recommendation**: Option A - defer to next event loop, allows batching multiple additions

### Strategy 4: Optimize Cursor Layer Updates

**Approach**: Track last cursor position and only update if position changed.

**Implementation**:
- Store last mouse position and cursor time
- Only set `needsUpdate = true` if values changed
- Reduces unnecessary repaints

### Strategy 5: Remove GraphContainer Redundant Calls

**Approach**: Remove explicit `draw()` call in `updateTimeInterval()` since `setTimeInterval()` already triggers it.

**Location**: `graphcontainer.cpp` line ~882

### Strategy 6: Batch Multiple State Changes

**Approach**: When multiple state changes occur in sequence, batch them into a single draw call.

**Implementation**:
- Track if draw is already scheduled
- Use QTimer::singleShot(0) to batch updates
- Only one draw per event loop iteration

## Recommended Implementation Priority

1. **High Priority**: Add view update to `drawIncremental()` - fixes missing repaints
2. **High Priority**: Remove redundant `draw()` calls from state setters
3. **Medium Priority**: Defer draw calls for data additions (batch updates)
4. **Medium Priority**: Remove GraphContainer redundant calls
5. **Low Priority**: Optimize cursor layer (already well optimized)

## Expected Impact

- **Before**: ~20-30 draw calls per second (with 2Hz data updates + cursor updates)
- **After**: ~2-4 draw calls per second (batched updates)
- **Reduction**: 80-90% fewer draw calls
- **Performance**: Lower CPU usage, smoother rendering

