#pragma once

#include <unordered_set>
#include <queue>

#include "SortedArray.h"

#include "Pixel.h"


//TODO : template for Vec3b
template<uint num_channels>
class Abstractor : public cv::ParallelLoopBody
{
private:
    /* Definitions */
    using VecNb = cv::Vec<unsigned char, num_channels>;

public:
    Abstractor();

    void init(const cv::Mat_<VecNb> *image);

    cv::Mat_< cv::Vec<unsigned char, num_channels> >* abstract(uint theMaskSize, double gamma);

private:
    using PriorityPixels = SortedArray;

    /*---------------*/
    /* Attributes */
    /*---------------*/
    uint maskSize;

    const cv::Mat_<VecNb> *image;
    cv::Mat_<VecNb> *abstracted_image;

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
    cv::Mat absolute_difference(const cv::Mat &image_a, const cv::Mat &image_b) const;
    cv::Mat_<ushort> calculate_difference (const cv::Mat &image_a, const cv::Mat &image_b) const;

    /* Abstraction */

    virtual void operator()(const cv::Range &range) const override;

    cv::Vec<unsigned char, num_channels> abstractPixel(uint maskSize, int row, int column) const;

    void select_originalNeighBors(int row, int column, std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const;


    /* Select Pixel */

    void select(cv::Vec<float, num_channels> &abstractedPixel, const VecNb &pixelOrigin, const Pixel &closestPixel,
                std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const;

    void add_neighborsToPossiblePixels (const VecNb &pixelOrigin, const Pixel &closestPixel,
                                        const std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const;

    float propagationDistance(const VecNb &pixelOrigin, int neighbor_row, int neighbor_column,
                                          float distanceFromOrigin, float neightbor_distance) const;
};

#include "Abstractor.hpp"
