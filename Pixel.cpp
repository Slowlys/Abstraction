#include "Pixel.hpp"


using namespace cv;



Pixel::Pixel(const Point coordinate, const float distanceFromOrigin) :
    coordinate{coordinate},
    distanceFromOrigin{distanceFromOrigin}
{
}


bool Pixel::operator>(const Pixel& pixel) const {
    return distanceFromOrigin > pixel.distanceFromOrigin;
}


float Pixel::distanceBetween(const Vec3b &pixelA, const Vec3b &pixelB) {
    const auto distance = norm(pixelA, pixelB, NORM_L1);
    return static_cast<float>(distance);
}


bool Pixel::select(Vec3f &abstractedPixel, const Mat_<Vec3b> &image, const Vec3b &pixelOrigin,
                   const Mat_<float> &horizontal_difference_scaled, const Mat_<float> &vertical_difference_scaled,
                   std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const {
    // coordinate y : row, coordinate x : column
    const int index = coordinate.y * image.cols + coordinate.x;

    bool selected = selectedIndices.count(index) == 0;
    if (selected) {
        abstractedPixel += image(coordinate);
        selectedIndices.insert(index);
        add_neighborsToPossiblePixels(image, pixelOrigin, horizontal_difference_scaled, vertical_difference_scaled, selectedIndices, possiblePixels);
    }

    return selected;
}


void Pixel::add_neighborsToPossiblePixels (const Mat_<Vec3b> &image, const Vec3b &pixelOrigin,
                                           const Mat_<float> &horizontal_difference_scaled, const Mat_<float> &vertical_difference_scaled,
                                           const std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const {
    int index;
    int column = coordinate.x, row = coordinate.y;

    index = (row - 1) * image.cols + column;
    if ((row > 0) && (selectedIndices.count(index) == 0)) {
        const Point neighbor{column, row - 1};
        const float distance = propagationDistance(image, pixelOrigin, neighbor, horizontal_difference_scaled(row - 1, column));
        possiblePixels.emplace(neighbor, distance);
    }

    index += 2 * image.cols;
    if ((row < image.rows - 1) && (selectedIndices.count(index) == 0)) {
        const Point neighbor{column, row + 1};
        const float distance = propagationDistance(image, pixelOrigin, neighbor, horizontal_difference_scaled(row, column));
        possiblePixels.emplace(neighbor, distance);
    }

    index = index - image.cols - 1;
    if ((column > 0) && (selectedIndices.count(index) == 0)) {
        const Point neighbor{column - 1, row};
        const float distance = propagationDistance(image, pixelOrigin, neighbor, vertical_difference_scaled(row, column - 1));
        possiblePixels.emplace(neighbor, distance);
    }

    index += 2;
    if ((column < image.cols - 1) && (selectedIndices.count(index) == 0)) {
        const Point neighbor{column + 1, row};
        const float distance = propagationDistance(image, pixelOrigin, neighbor, vertical_difference_scaled(row, column));
        possiblePixels.emplace(neighbor, distance);
    }
}


float Pixel::propagationDistance(const Mat_<Vec3b> &image, const Vec3b &pixelOrigin, const Point &neighbor, float neightbor_distance) const {
    return distanceFromOrigin + distanceBetween(image(neighbor), pixelOrigin) + neightbor_distance;
}
