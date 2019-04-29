#include <stdio.h>
#include <opencv2/opencv.hpp>

#include <queue>
#include <unordered_set>

#include "Pixel.hpp"

using namespace cv;


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
    const char *image_path = "../images/chat.jpg";
    const char *window_name = "Abstraction";
    const char *output_path = "result.png";
    const uint maskSize = 300;
    const float gamma = 1.0f;

    Mat image = imread( image_path, IMREAD_UNCHANGED );
    if ( image.empty() ) {
        std::cerr << "The path of the image : " << image_path << " was not found" << std::endl;
        exit(EXIT_FAILURE);
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
            double time = static_cast<double>(getTickCount());

            Mat_<Vec3b> image3d = image;

            // Convert to HSV Space
            Mat image_hsv;
            image.convertTo(image_hsv, COLOR_BGR2HSV);

            // Hot ColorMap
            Mat image_hot, image_hotChar;
            image.convertTo(image_hot, COLORMAP_HOT);
            image_hot.convertTo(image_hotChar, CV_8U);

            namedWindow(window_name, WINDOW_AUTOSIZE );

            // Let's begin
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

            imshow(window_name, abstracted_image);
            waitKey(0);

            imwrite(output_path, abstracted_image);

            break;
        }
    }

    exit(EXIT_SUCCESS);
}
