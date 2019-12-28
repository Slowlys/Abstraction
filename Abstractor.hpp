#include "Abstractor.h"



using namespace std;
using namespace cv;


/* -------------------------- Init ------------------------------------------------*/

template<uint num_channels>
Abstractor<num_channels>::Abstractor() {

}

template<uint num_channels>
void Abstractor<num_channels>::init(const cv::Mat_<VecNb> *theImage) {
    image = theImage;

    calculate_horizontalVertical_difference();
}


template<uint num_channels>
void Abstractor<num_channels>::calculate_horizontalVertical_difference () {
    const Mat image_withoutLastRow = (*image)(Range{0, image->rows - 1}, Range::all());
    const Mat image_withoutFirstRow = (*image)(Range{1, image->rows}, Range::all());
    horizontal_difference = calculate_difference(image_withoutLastRow, image_withoutFirstRow);

    const Mat image_withoutLastColumn = (*image)(Range::all(), Range{0, image->cols - 1});
    const Mat image_withoutFirstColumn = (*image)(Range::all(), Range{1, image->cols});
    vertical_difference = calculate_difference(image_withoutLastColumn, image_withoutFirstColumn);
}


template<uint num_channels>
Mat Abstractor<num_channels>::absolute_difference (const Mat &image_a, const Mat &image_b) const {
    Mat difference;
    absdiff(image_a, image_b, difference);

    return difference;
}


template<uint num_channels>
Mat_<ushort> Abstractor<num_channels>::calculate_difference (const Mat &image_a, const Mat &image_b) const {
    Mat difference = absolute_difference(image_a, image_b);

    // Convert to unsigned int so that the addition of channels will be stored in a sufficient number of bits
    Mat difference_16U;
    difference.convertTo(difference_16U, CV_16U);

    // Sum the channels of each pixels
    Mat difference_oneChannel;
    transform(difference_16U, difference_oneChannel, Matx<float, 1, num_channels>(1,1,1));

    return difference_oneChannel;
}


template<>
Mat_<ushort> Abstractor<1>::calculate_difference (const Mat &image_a, const Mat &image_b) const {
    return absolute_difference(image_a, image_b);
}


/*-------------------------------- Abstraction ------------------------------------------------*/


template<uint num_channels>
cv::Mat_<cv::Vec<unsigned char, num_channels> > *Abstractor<num_channels>::abstract(uint theMaskSize, double gamma) {
    maskSize = theMaskSize;

    horizontal_difference.convertTo(horizontal_difference_scaled, CV_32F, gamma);
    vertical_difference.convertTo(vertical_difference_scaled, CV_32F, gamma);

    abstracted_image = new Mat_<VecNb>{image->size(), CV_8U};
    parallel_for_(Range{0, image->rows * image->cols}, *this);

    return abstracted_image;
}


template<uint num_channels>
void Abstractor<num_channels>::operator()(const cv::Range &range) const {
    for (int index = range.start; index < range.end; ++index) {
        const int row = index / image->cols;
        const int column = index % image->cols;

        (*abstracted_image)(row, column) = abstractPixel(maskSize, row, column);
    }
}


template<>
void Abstractor<4>::operator()(const cv::Range &range) const {
    for (int index = range.start; index < range.end; ++index) {
        const int row = index / image->cols;
        const int column = index % image->cols;

        if ( (*image)(row, column)[3] > 0.0 ) {
            (*abstracted_image)(row, column) = abstractPixel(maskSize, row, column);
        } else {
            (*abstracted_image)(row, column) = Vec4b{0, 0, 0, 0};
        }
    }
}


template<uint num_channels>
Vec<unsigned char, num_channels> Abstractor<num_channels>::abstractPixel (uint maskSize, int row, int column) const {
    unordered_set<int> selectedIndices;
    PriorityPixels possiblePixels{static_cast<int>(maskSize - 1)};

    const VecNb &pixelOrigin = (*image)(row, column);
    Vec<float, num_channels> abstractedPixel = pixelOrigin;

    select_originalNeighBors(row, column, selectedIndices, possiblePixels);

    for (uint i = 1; i < maskSize - 1 ; ++i) {
        const Pixel closestPixel = possiblePixels.extractMin();
        select(abstractedPixel, pixelOrigin, closestPixel, selectedIndices, possiblePixels);
    }

    // Add last pixel
    const Pixel closestPixel = possiblePixels.extractMin();
    const int lastRow = closestPixel.getRow();
    const int lastColumn = closestPixel.getColumn();
    abstractedPixel += (*image)(lastRow, lastColumn);

    return abstractedPixel / static_cast<float>(maskSize);
}


// Put the neighbors of the original pixel into the queue
template<uint num_channels>
void Abstractor<num_channels>::select_originalNeighBors(int row, int column,
                              unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const {
    selectedIndices.insert(row * image->cols + column);

    if (row > 0) {
        possiblePixels.emplace(row - 1, column, horizontal_difference(row - 1, column));
    }

    if (row < image->rows - 1) {
        possiblePixels.emplace(row + 1, column, horizontal_difference(row, column));
    }

    if (column > 0) {
        possiblePixels.emplace(row, column - 1, vertical_difference(row, column - 1));
    }

    if (column < image->cols - 1) {
        possiblePixels.emplace(row, column + 1, vertical_difference(row, column));
    }
}

/*-------------------------------- Select Pixel ------------------------------------------------*/

template<uint num_channels>
void Abstractor<num_channels>::select(Vec<float, num_channels> &abstractedPixel, const VecNb &pixelOrigin, const Pixel &closestPixel,
                   std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const {
    const int row = closestPixel.getRow();
    const int column = closestPixel.getColumn();
    const int index = row * image->cols + column;

    abstractedPixel += (*image)(row, column);

    selectedIndices.insert(index);
    add_neighborsToPossiblePixels(pixelOrigin, closestPixel, selectedIndices, possiblePixels);
}


template<uint num_channels>
void Abstractor<num_channels>::add_neighborsToPossiblePixels (const VecNb &pixelOrigin, const Pixel &closestPixel,
                                                const std::unordered_set<int> &selectedIndices, PriorityPixels &possiblePixels) const {
    int index;
    const float distanceFromOrigin = closestPixel.getDistanceFromOrigin();
    const int column = closestPixel.getColumn();
    const int row = closestPixel.getRow();

    index = (row - 1) * image->cols + column;
    if ((row > 0) && (selectedIndices.count(index) == 0)) {
        const float distance = propagationDistance(pixelOrigin, row - 1, column, distanceFromOrigin,
                                                   horizontal_difference_scaled(row - 1, column));
        possiblePixels.emplace(row - 1, column, distance);
    }

    index += 2 * image->cols;
    if ((row < image->rows - 1) && (selectedIndices.count(index) == 0)) {
        const float distance = propagationDistance(pixelOrigin, row + 1, column, distanceFromOrigin,
                                                   horizontal_difference_scaled(row, column));
        possiblePixels.emplace(row + 1, column, distance);
    }

    index = index - image->cols - 1;
    if ((column > 0) && (selectedIndices.count(index) == 0)) {
        const float distance = propagationDistance(pixelOrigin, row, column - 1, distanceFromOrigin,
                                                   vertical_difference_scaled(row, column - 1));
        possiblePixels.emplace(row, column - 1, distance);
    }

    index += 2;
    if ((column < image->cols - 1) && (selectedIndices.count(index) == 0)) {
        const float distance = propagationDistance(pixelOrigin, row, column + 1, distanceFromOrigin,
                                                   vertical_difference_scaled(row, column));
        possiblePixels.emplace(row, column + 1, distance);
    }
}


template<uint num_channels>
float Abstractor<num_channels>::propagationDistance(const VecNb &pixelOrigin, int neighbor_row, int neighbor_column,
                                      float distanceFromOrigin, float neightbor_distance) const {
    return distanceFromOrigin + Pixel::distanceBetween<num_channels>((*image)(neighbor_row, neighbor_column), pixelOrigin) + neightbor_distance;
}
