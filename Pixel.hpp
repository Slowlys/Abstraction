#pragma once

#include <opencv2/opencv.hpp>

class Pixel
{

public:
    Pixel(int row, int column, float distanceFromOrigin);

    inline int getRow() const { return row; }
    inline int getColumn() const { return column; }
    inline bool asSameCoordinates (int theRow, int theColumn) const { return (theRow == row) && (theColumn == column); }

    inline float getDistanceFromOrigin() const { return distanceFromOrigin; }

    bool operator>(const Pixel &pixel) const;

    static float distanceBetween(const cv::Vec3b &pixelA, const cv::Vec3b &pixelB);

private:
    /*---------------*/
    /* Attributes */
    /*---------------*/

    int row, column;
    float distanceFromOrigin;
};









