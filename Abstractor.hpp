#pragma once

#include <unordered_set>
#include <queue>

#include "DoubledLinkedList.hpp"
#include "UnrolledLinkedList.hpp"

#include "Pixel.hpp"


//TODO : template for Vec3b
class Abstractor
{
private:
    /* Definitions */
    struct CloserThanPixel {
        bool operator()(const Pixel &pixelA, const Pixel &pixelB) const{
            return pixelA > pixelB;
        }
    };
public:
    using PriorityPixels = UnrolledLinkedList;
    //using PriorityPixels = DoubledLinkedList;
    //using PriorityPixels = std::priority_queue<Pixel&, std::vector<Pixel>, CloserThanPixel>;

    Abstractor();

    void init(const cv::Mat_<cv::Vec3b> *image);

    cv::Mat_<cv::Vec3b> abstract(uint maskSize, double gamma);

private:
    /*---------------*/
    /* Attributes */
    /*---------------*/
    const cv::Mat_<cv::Vec3b> *image;

    /*
     * horizontal_difference : At (i, j) : the difference between (i, j) and (i+1, j)
     * vertical_difference : At (i, j) : the difference between (i, j) and (i, j+1)
     */
    cv::Mat_<ushort> horizontal_difference, vertical_difference;
    cv::Mat_<float> horizontal_difference_scaled, vertical_difference_scaled;

    /*---------------*/
    /* Functions */
    /*---------------*/

    /* Init */

    void calculate_horizontalVertical_difference ();
    cv::Mat_<ushort> calculate_difference (const cv::Mat &image_a, const cv::Mat &image_b) const;

    /* Abstraction */

    cv::Vec3b abstractPixel (uint maskSize, int row, int column) const;
    void select_originalNeighBors(int row, int column, std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const;


    /* Select Pixel */

    void select(cv::Vec3f &abstractedPixel, const cv::Vec3b &pixelOrigin, const Pixel &closestPixel,
                std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const;

    void add_neighborsToPossiblePixels (const cv::Vec3b &pixelOrigin, const Pixel &closestPixel,
                                        const std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const;

    float propagationDistance(const cv::Vec3b &pixelOrigin, int neighbor_row, int neighbor_column,
                                          float distanceFromOrigin, float neightbor_distance) const;
};
