#pragma once

#include <opencv2/opencv.hpp>

class Pixel
{

public:
    Pixel(const cv::Point coordinate, float distanceFromOrigin);

    inline const cv::Point& getCoordinate() const { return coordinate; }

    inline float getDistanceFromOrigin() const { return distanceFromOrigin; }

    bool operator>(const Pixel &pixel) const;

    static float distanceBetween(const cv::Vec3b &pixelA, const cv::Vec3b &pixelB);

private:
    /*---------------*/
    /* Attributes */
    /*---------------*/

    // coordinate y : row, coordinate x : column
    cv::Point coordinate;

    float distanceFromOrigin;
};









