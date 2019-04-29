#pragma once

#include <unordered_set>
#include <queue>
#include <opencv2/opencv.hpp>

class Pixel
{
private:
    /* Definitions */
    struct CloserThanPixel {
        bool operator()(const Pixel &pixelA, const Pixel &pixelB) const{
            return pixelA > pixelB;
        }
    };

public:
    using PriorityPixels = std::priority_queue<const Pixel&, std::vector<Pixel>, CloserThanPixel>;

    Pixel(float gamma, const cv::Point coordinate, float distanceFromOrigin);

    bool operator>(const Pixel &pixel) const;

    static int distanceBetween(const cv::Vec3b &pixelA, const cv::Vec3b &pixelB);

    bool select(cv::Vec3f &abstractedPixel, const cv::Mat_<cv::Vec3b> &image, const cv::Vec3b &pixelOrigin,
                std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const;

private:
    /* Attributes */
    // TODO : static
    float gamma;

    cv::Point coordinate;
    float distanceFromOrigin;

    /* Functions */

    void add_neighborsToPossiblePixels (const cv::Mat_<cv::Vec3b> &image, const cv::Vec3b &pixelOrigin,
                                        const std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const;

    float propagationDistance(const cv::Mat_<cv::Vec3b> &image, const cv::Vec3b &pixelOrigin, const cv::Vec3b &pixelValue, const cv::Point &neighbor) const;
};









