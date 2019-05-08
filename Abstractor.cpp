#include "Abstractor.hpp"




using namespace std;
using namespace cv;


/* -------------------------- Init ------------------------------------------------*/

Abstractor::Abstractor() {

}

void Abstractor::init(const cv::Mat_<cv::Vec3b> *theImage) {
    image = theImage;

    calculate_horizontalVertical_difference();
}


void Abstractor::calculate_horizontalVertical_difference () {
    const Mat image_withoutLastRow = (*image)(Range{0, image->rows - 1}, Range::all());
    const Mat image_withoutFirstRow = (*image)(Range{1, image->rows}, Range::all());
    horizontal_difference = calculate_difference(image_withoutLastRow, image_withoutFirstRow);

    const Mat image_withoutLastColumn = (*image)(Range::all(), Range{0, image->cols - 1});
    const Mat image_withoutFirstColumn = (*image)(Range::all(), Range{1, image->cols});
    vertical_difference = calculate_difference(image_withoutLastColumn, image_withoutFirstColumn);
}


Mat_<ushort> Abstractor::calculate_difference (const Mat &image_a, const Mat &image_b) const {
    // Absolute Difference
    Mat difference;
    absdiff(image_a, image_b, difference);

    // Convert to unsigned int so that the addition of channels will be stored in a sufficient number of bits
    Mat difference_16U;
    difference.convertTo(difference_16U, CV_16U);

    // Sum the channels of each pixels
    Mat difference_oneChannel;
    transform(difference_16U, difference_oneChannel, Matx13f(1,1,1));

    return difference_oneChannel;
}


/*-------------------------------- Abstraction ------------------------------------------------*/


Mat_<Vec3b> Abstractor::abstract(uint maskSize, double gamma) {
    //Mat_<Vec3b> abstracted_image{image.size(), CV_8U};
    Mat_<Vec3b> abstracted_image = image->clone();

    horizontal_difference.convertTo(horizontal_difference_scaled, CV_32F, gamma);
    vertical_difference.convertTo(vertical_difference_scaled, CV_32F, gamma);

    for (int row = 0; row < image->rows; ++row) {
        for (int column = 0; column < image->cols; ++column) {
            abstracted_image(row, column) = abstractPixel(maskSize, row, column);
        }

        imshow("Abstraction", abstracted_image);
        waitKey(1);
    }

    return abstracted_image;
}


Vec3b Abstractor::abstractPixel (uint maskSize, int row, int column) const {
    unordered_set<int> selectedIndices;
    PriorityPixels possiblePixels;

    const Vec3b &pixelOrigin = (*image)(row, column);
    Vec3f abstractedPixel = pixelOrigin;

    select_originalNeighBors(row, column, selectedIndices, possiblePixels);

    for (uint i = 1; i < maskSize; ++i) {
        bool newPixelAdded = false;
        do {
            const Pixel closestPixel = possiblePixels.extractMin();
            newPixelAdded = select(abstractedPixel, pixelOrigin, closestPixel,
                                   selectedIndices, possiblePixels);
        } while (!newPixelAdded);
    }

    return abstractedPixel / static_cast<float>(maskSize);
}


// Put the neighbors of the original pixel into the queue
void Abstractor::select_originalNeighBors(int row, int column,
                              unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const {
    selectedIndices.insert(row * image->cols + column);

    if (row > 0) {
        const Point coordinate{column, row - 1};
        possiblePixels.emplace(coordinate, horizontal_difference(row - 1, column));
    }

    if (row < image->rows - 1) {
        const Point coordinate{column, row + 1};
        possiblePixels.emplace(coordinate, horizontal_difference(row, column));
    }

    if (column > 0) {
        const Point coordinate{column - 1, row};
        possiblePixels.emplace(coordinate, vertical_difference(row, column - 1));
    }

    if (column < image->cols - 1) {
        const Point coordinate{column + 1, row};
        possiblePixels.emplace(coordinate, vertical_difference(row, column));
    }
}

/*-------------------------------- Select Pixel ------------------------------------------------*/

bool Abstractor::select(Vec3f &abstractedPixel, const Vec3b &pixelOrigin, const Pixel &closestPixel,
                   std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const {
    const Point &coordinate = closestPixel.getCoordinate();
    const int index = coordinate.y * image->cols + coordinate.x;

    bool selected = selectedIndices.count(index) == 0;
    if (selected) {
        abstractedPixel += (*image)(coordinate);
        selectedIndices.insert(index);
        add_neighborsToPossiblePixels(pixelOrigin, closestPixel, selectedIndices, possiblePixels);
    }

    return selected;
}


void Abstractor::add_neighborsToPossiblePixels (const Vec3b &pixelOrigin, const Pixel &closestPixel,
                                                const std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const {
    int index;
    const float distanceFromOrigin = closestPixel.getDistanceFromOrigin();
    const Point &coordinate = closestPixel.getCoordinate();
    int column = coordinate.x, row = coordinate.y;

    index = (row - 1) * image->cols + column;
    if ((row > 0) && (selectedIndices.count(index) == 0)) {
        const Point neighbor{column, row - 1};
        const float distance = propagationDistance(pixelOrigin, neighbor, distanceFromOrigin,
                                                   horizontal_difference_scaled(row - 1, column));
        possiblePixels.emplace_nonEmpty(neighbor, distance);
    }

    index += 2 * image->cols;
    if ((row < image->rows - 1) && (selectedIndices.count(index) == 0)) {
        const Point neighbor{column, row + 1};
        const float distance = propagationDistance(pixelOrigin, neighbor, distanceFromOrigin,
                                                   horizontal_difference_scaled(row, column));
        possiblePixels.emplace_nonEmpty(neighbor, distance);
    }

    index = index - image->cols - 1;
    if ((column > 0) && (selectedIndices.count(index) == 0)) {
        const Point neighbor{column - 1, row};
        const float distance = propagationDistance(pixelOrigin, neighbor, distanceFromOrigin,
                                                   vertical_difference_scaled(row, column - 1));
        possiblePixels.emplace_nonEmpty(neighbor, distance);
    }

    index += 2;
    if ((column < image->cols - 1) && (selectedIndices.count(index) == 0)) {
        const Point neighbor{column + 1, row};
        const float distance = propagationDistance(pixelOrigin, neighbor, distanceFromOrigin,
                                                   vertical_difference_scaled(row, column));
        possiblePixels.emplace_nonEmpty(neighbor, distance);
    }
}


float Abstractor::propagationDistance(const Vec3b &pixelOrigin, const Point &neighbor,
                                      float distanceFromOrigin, float neightbor_distance) const {
    return distanceFromOrigin + Pixel::distanceBetween((*image)(neighbor), pixelOrigin) + neightbor_distance;
}
