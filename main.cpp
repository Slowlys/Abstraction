#include <stdio.h>
#include <opencv2/opencv.hpp>

#include <queue>
#include <unordered_set>

#include "Pixel.hpp"

using namespace cv;



Mat calculate_difference (const Mat &image_a, const Mat &image_b) {
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

/*
 * horizontal_difference : At (i, j) : the difference between (i, j) and (i+1, j)
 * vertical_difference : At (i, j) : the difference between (i, j) and (i, j+1)
 */
void calculate_horizontalVertical_difference (const Mat &image, Mat &horizontal_difference, Mat &vertical_difference) {
    const Mat image_withoutLastRow = image(Range{0, image.rows - 2}, Range::all());
    const Mat image_withoutFirstRow = image(Range{1, image.rows - 1}, Range::all());
    horizontal_difference = calculate_difference(image_withoutLastRow, image_withoutFirstRow);

    const Mat image_withoutLastColumn = image(Range::all(), Range{0, image.cols - 2});
    const Mat image_withoutFirstColumn = image(Range::all(), Range{1, image.cols - 1});
    vertical_difference = calculate_difference(image_withoutLastColumn, image_withoutFirstColumn);
}


void display_difference(const Mat &horizontal_difference, const Mat &vertical_difference, const char *display_name) {
    const Mat horizontal_difference_resized = horizontal_difference(Range::all(), Range{0, horizontal_difference.cols - 2});
    const Mat vertical_difference_resized = vertical_difference(Range{0, vertical_difference.rows - 2}, Range::all());
    const Mat difference = horizontal_difference_resized + vertical_difference_resized;

    Mat difference_char;
    difference.convertTo(difference_char, CV_8U, (255.0 / 1530.0));

    Mat difference_pink;
    applyColorMap(difference_char, difference_pink, COLORMAP_PINK);

    imshow(display_name, difference_pink);
}


// Put the neighbors of the original pixel into the queue
void select_originalNeighBors(const Mat_<Vec3b> &image, const Vec3b &pixelOrigin, int row, int column, const float gamma,
                              std::unordered_set<int> &selectedIndices, Pixel::PriorityPixels &possiblePixels) {
    selectedIndices.insert(row * image.cols + column);

    if (row > 0) {
        const Point coordinate{column, row - 1};
        const auto distance = Pixel::distanceBetween(image(coordinate), pixelOrigin);
        possiblePixels.emplace(gamma, coordinate, distance);
    }

    if (row < image.rows - 1) {
        const Point coordinate{column, row + 1};
        const auto distance = Pixel::distanceBetween(image(coordinate), pixelOrigin);
        possiblePixels.emplace(gamma, coordinate, distance);
    }

    if (column > 0) {
        const Point coordinate{column - 1, row};
        const auto distance = Pixel::distanceBetween(image(coordinate), pixelOrigin);
        possiblePixels.emplace(gamma, coordinate, distance);
    }

    if (column < image.cols - 1) {
        const Point coordinate{column + 1, row};
        const auto distance = Pixel::distanceBetween(image(coordinate), pixelOrigin);
        possiblePixels.emplace(gamma, coordinate, distance);
    }
}


Vec3b abstractPixel (uint maskSize, const float gamma, const Mat_<Vec3b> &image, int row, int column) {
    std::unordered_set<int> selectedIndices;
    Pixel::PriorityPixels possiblePixels;

    const Vec3b &pixelOrigin = image(row, column);
    Vec3f abstractedPixel = pixelOrigin;

    select_originalNeighBors(image, pixelOrigin, row, column, gamma, selectedIndices, possiblePixels);

    for (uint i = 1; i < maskSize; ++i) {
        bool newPixelAdded = false;
        do {
            const Pixel closestPixel = possiblePixels.top();
            possiblePixels.pop();
            newPixelAdded = closestPixel.select(abstractedPixel, image, pixelOrigin, selectedIndices, possiblePixels);
        } while (!newPixelAdded);
    }

    return abstractedPixel / static_cast<float>(maskSize);
}


int main() {
    const char *image_path = "../images/herisson.jpg";
    const char *window_name = "Abstraction";
    const char *output_path = "result.png";
    const uint maskSize = 300;
    const float gamma = 1.0f;

    Mat image = imread( image_path, IMREAD_UNCHANGED );
    if ( image.empty() ) {
        CV_Error(CV_StsBadArg, std::string{"The path of the image : "} + image_path + std::string{" was not found"});
    }

    // Accept only char type matrices
    // TODO : Faire un message d'erreur
    CV_Assert(image.depth() == CV_8U);

    const int num_channels = image.channels();

    switch (num_channels) {
        case 1:
        {
            MatIterator_<uchar> end = image.end<uchar>();
            for (auto iterator = image.begin<uchar>(); iterator != end; ++iterator) {
                *iterator = 0;
            }

            break;
        }

        case 3 :
        {            
            Mat_<Vec3b> image3d = image;

            // Convert to HSV Space
            Mat image_hsv;
            image.convertTo(image_hsv, COLOR_BGR2HSV);

            // Hot ColorMap
            Mat image_hot, image_hotChar;
            image.convertTo(image_hot, COLORMAP_HOT);
            image_hot.convertTo(image_hotChar, CV_8U);

            // Let's begin
            Mat horizontal_difference, vertical_difference;
            calculate_horizontalVertical_difference (image, horizontal_difference, vertical_difference);

            double time = static_cast<double>(getTickCount());

            //Mat_<Vec3b> abstracted_image{image.size(), CV_8U};
            Mat_<Vec3b> abstracted_image = image.clone();

            for (int row = 0; row < image3d.rows; ++row) {
                for (int column = 0; column < image3d.cols; ++column) {
                    abstracted_image(row, column) = abstractPixel(maskSize, gamma, image3d, row, column);
                }

                imshow(window_name, abstracted_image);
                waitKey(1);
            }

            time = static_cast<double>(getTickCount()) - time;
            const double time_seconds = time / getTickFrequency();
            std::cout << "Number of Ticks : " << time << " Seconds : " << time_seconds << std::endl;

            Mat abstract_horizontal_difference, abstract_vertical_difference;
            calculate_horizontalVertical_difference (abstracted_image, abstract_horizontal_difference, abstract_vertical_difference);

            imshow(window_name, abstracted_image);
            display_difference(horizontal_difference, vertical_difference, "Difference Before");
            display_difference(abstract_horizontal_difference, abstract_vertical_difference, "Difference After");

            waitKey(0);

            imwrite(output_path, abstracted_image);

            break;
        }
    }

    exit(EXIT_SUCCESS);
}
