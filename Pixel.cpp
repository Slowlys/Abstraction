#include "Pixel.hpp"


using namespace cv;



Pixel::Pixel(const Point coordinate, float distanceFromOrigin) :
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

