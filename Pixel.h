#pragma once

#include <opencv2/opencv.hpp>

class Pixel
{

public:
    Pixel();

    void init(int theRow, int theColumn, float theDistanceFromOrigin);

    inline int getRow() const { return row; }
    inline int getColumn() const { return column; }
    inline bool asSameCoordinates (int theRow, int theColumn) const { return (theRow == row) && (theColumn == column); }

    inline float getDistanceFromOrigin() const { return distanceFromOrigin; }

    bool operator>(const Pixel &pixel) const;

    template<uint num_channels>
    static float distanceBetween(const cv::Vec<unsigned char, num_channels> &pixelA, const cv::Vec<unsigned char, num_channels> &pixelB) {
        const auto distance = norm(pixelA, pixelB, cv::NORM_L1);
        return static_cast<float>(distance);
    }

private:
    /*---------------*/
    /* Attributes */
    /*---------------*/

    int row, column;
    float distanceFromOrigin;
};










