#ifndef TWOAXISDATA_H
#define TWOAXISDATA_H

#include <vector>

class TwoAxisData {
private:
	std::vector<double> x_data;
	std::vector<double> y1_data;
	std::vector<double> y2_data;

public:
	// Getters
	const std::vector<double>& getXData() const { return x_data; }
	const std::vector<double>& getY1Data() const { return y1_data; }
	const std::vector<double>& getY2Data() const { return y2_data; }

	// Setters
    bool setData(const std::vector<double>& x, const std::vector<double>& y1, const std::vector<double>& y2) {

        if (x.size() != y1.size() || x.size() != y2.size()) {
            return false;  // Vectors must have the same dimension
        }
        x_data = x;
        y1_data = y1;
        y2_data = y2;
        return true;
    }
};
#endif // TWOAXISDATA_H

