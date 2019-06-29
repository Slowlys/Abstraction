#include <stdio.h>
#include <opencv2/opencv.hpp>

#include "Abstractor.h"



using namespace cv;


void display_difference(const Mat_<ushort> &horizontal_difference, const Mat_<ushort> &vertical_difference, const char *display_name) {
    const Mat horizontal_difference_resized = horizontal_difference(Range::all(), Range{0, horizontal_difference.cols - 1});
    const Mat vertical_difference_resized = vertical_difference(Range{0, vertical_difference.rows - 1}, Range::all());
    const Mat difference = horizontal_difference_resized + vertical_difference_resized;

    Mat difference_char;
    difference.convertTo(difference_char, CV_8U, (255.0 / 1530.0));

    Mat difference_pink;
    applyColorMap(difference_char, difference_pink, COLORMAP_PINK);

    imshow(display_name, difference_pink);
}


int main() {
    const char *image_path = "../images/herisson.jpg";
    const char *window_name = "Abstraction";
    const char *output_path = "result.png";
    const uint maskSize = 100;
    const double gamma = 1.0;

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
            Mat_< Vec<unsigned char, 1> > image1d = image;

            // Let's begin
            double time = static_cast<double>(getTickCount());

            Abstractor<1> abstractor;
            abstractor.init(&image1d);
            const Mat_< Vec<unsigned char, 1> > *abstracted_image = abstractor.abstract(maskSize, gamma);

            time = static_cast<double>(getTickCount()) - time;
            const double time_seconds = time / getTickFrequency();
            std::cout << "Seconds : " << time_seconds << std::endl;

            imshow(window_name, *abstracted_image);
            waitKey(0);

            imwrite(output_path, *abstracted_image);

            delete abstracted_image;
            break;
        }

        case 3 :
        {                       
            // Convert to HSV Space
            Mat image_hsv;
            image.convertTo(image_hsv, COLOR_BGR2HSV);

            Mat_<Vec3b> image3d = image;

            // Let's begin
            double time = static_cast<double>(getTickCount());

            Abstractor<3> abstractor;
            abstractor.init(&image3d);
            const Mat_<Vec3b> *abstracted_image = abstractor.abstract(maskSize, gamma);

            time = static_cast<double>(getTickCount()) - time;
            const double time_seconds = time / getTickFrequency();
            std::cout << "Seconds : " << time_seconds << std::endl;

            imshow(window_name, *abstracted_image);
            waitKey(0);

            imwrite(output_path, *abstracted_image);

            delete abstracted_image;
            break;
        }

        case 4 :
        {
            Mat_<Vec4b> image4d = image;

            // Let's begin
            double time = static_cast<double>(getTickCount());

            Abstractor<4> abstractor;
            abstractor.init(&image4d);
            const Mat_<Vec4b> *abstracted_image = abstractor.abstract(maskSize, gamma);

            time = static_cast<double>(getTickCount()) - time;
            const double time_seconds = time / getTickFrequency();
            std::cout << "Seconds : " << time_seconds << std::endl;

            imshow(window_name, *abstracted_image);
            waitKey(0);

            imwrite(output_path, *abstracted_image);

            delete abstracted_image;
            break;
        }
    }

    exit(EXIT_SUCCESS);
}
