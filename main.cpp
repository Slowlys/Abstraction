#include <stdio.h>
#include <opencv2/opencv.hpp>


#include "Abstractor.h"



using namespace cv;


template<uint num_channels>
void abstract (Abstractor<num_channels> &abstractor, const Mat_< Vec<unsigned char, num_channels> > &imageNd,
          const Mat_< Vec<unsigned char, num_channels> > *abstracted_image,
          const uint maskSize, const double gamma, const char *output_path) {

    double time = static_cast<double>(getTickCount());

    abstractor.init(&imageNd);
    abstracted_image = abstractor.abstract(maskSize, gamma);

    time = static_cast<double>(getTickCount()) - time;
    const double time_seconds = time / getTickFrequency();
    std::cout << "Seconds : " << time_seconds << std::endl;

    imshow("Abstraction", *abstracted_image);
    waitKey(0);

    imwrite(output_path, *abstracted_image);
}


void print_welcome_message (const char *program_name) {
    std::cerr << "Hello !" << std::endl <<
                "To abstract an image," << std::endl <<
                "Please provide the path of this image " <<
                "with : " + string{program_name} + " -image_path [image_path]" <<
                 std::endl << std::endl <<
                 "You can also provide other optional arguments :" << std::endl <<
                 "Usage : " << program_name << " -image_path [image_path] -output_path [output_path] "
                                               "-maskSize [maskSize] -gamma [gamma]" << std::endl;
}


void parse_arguments(int num_arguments, char *arguments[],
                     char* &image_path, const char* &output_path, uint &maskSize, double &gamma) {
    if (num_arguments < 2 || strcmp(arguments[1], "-h") == 0 || strcmp(arguments[1], "-help") == 0) {
        print_welcome_message(arguments[0]);
        exit(EXIT_SUCCESS);
    } else {
        for (int i = 1; i < num_arguments; i += 2) {
            if (strcmp(arguments[i], "-image_path") == 0) {
                image_path = arguments[i + 1];
            }

            if (strcmp(arguments[i], "-output_path") == 0) {
                output_path = arguments[i + 1];
            }

            if (strcmp(arguments[i], "-maskSize") == 0) {
                maskSize = static_cast<uint>(std::stoul(arguments[i + 1]));
            }

            if (strcmp(arguments[i], "-gamma") == 0) {
                gamma = std::stod(arguments[i + 1]);
            }
        }

        if (image_path == nullptr) {
            std::cerr << "The image_path argument was not given." << std::endl;
            print_welcome_message(arguments[0]);
            exit(EXIT_FAILURE);
        }
    }
}


int main(int argc, char *argv[]) {
    char *image_path = nullptr;
    const char *output_path = "result.png";
    uint maskSize = 100;
    double gamma = 1.0;

    parse_arguments(argc, argv, image_path, output_path, maskSize, gamma);

    Mat image = imread( image_path, IMREAD_UNCHANGED );
    if ( image.empty() ) {
        CV_Error(CV_StsBadArg, std::string{"The path of the image : "} + image_path + std::string{" was not found"});
    }
    
    const int num_channels = image.channels();

    switch (num_channels) {
        case 1:
        {
            Mat_< Vec<unsigned char, 1> > image1d = image;
            Abstractor<1> abstractor;
            const Mat_< Vec<unsigned char, 1> > *abstracted_image = nullptr;

            abstract<1>(abstractor, image1d, abstracted_image, maskSize, gamma, output_path);

            delete abstracted_image;
            break;
        }
        case 3 :
        {                       
            Mat_<Vec3b> image3d = image;
            Abstractor<3> abstractor;
            const Mat_<Vec3b> *abstracted_image = nullptr;

            abstract<3>(abstractor, image3d, abstracted_image, maskSize, gamma, output_path);

            delete abstracted_image;
            break;
        }
        case 4 :
        {
            Mat_<Vec4b> image4d = image;
            Abstractor<4> abstractor;
            const Mat_<Vec4b> *abstracted_image = nullptr;

            abstract<4>(abstractor, image4d, abstracted_image, maskSize, gamma, output_path);

            delete abstracted_image;
            break;
        }
        default :
        {
            std::cerr << "Image Type not supported." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
