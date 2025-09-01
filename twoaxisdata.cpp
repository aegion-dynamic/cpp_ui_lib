#include "twoaxisdata.h"

bool TwoAxisData::setData(const std::vector<double> &x, const std::vector<double> &y1, const std::vector<double> &y2)
{
    if (x.size() != y1.size() || x.size() != y2.size())
    {
        return false; // Vectors must have the same dimension
    }
    x_data = x;
    y1_data = y1;
    y2_data = y2;
    updateRanges(); // Update ranges when data changes
    return true;
}

double TwoAxisData::getY1AtX(double x) const
{
    // TODO:Implement linear interpolation or direct access
    return 0.0; // Placeholder
}

double TwoAxisData::getY2AtX(double x) const
{
    // TODO:Implement linear interpolation or direct access
    return 0.0; // Placeholder
}

void TwoAxisData::updateRanges()
{
    if (x_data.empty())
    {
        x_range = y1_range = y2_range = Range{};
        return;
    }

    // Find X range
    auto [xMinIt, xMaxIt] = std::minmax_element(x_data.begin(), x_data.end());
    x_range.min = *xMinIt;
    x_range.max = *xMaxIt;

    // Find Y1 range
    auto [y1MinIt, y1MaxIt] = std::minmax_element(y1_data.begin(), y1_data.end());
    y1_range.min = *y1MinIt;
    y1_range.max = *y1MaxIt;

    // Find Y2 range
    auto [y2MinIt, y2MaxIt] = std::minmax_element(y2_data.begin(), y2_data.end());
    y2_range.min = *y2MinIt;
    y2_range.max = *y2MaxIt;

    // Add padding (5%)
    auto addPadding = [](Range &r)
    {
        double pad = (r.max - r.min) * 0.05;
        r.min -= pad;
        r.max += pad;
    };

    addPadding(x_range);
    addPadding(y1_range);
    addPadding(y2_range);
}
