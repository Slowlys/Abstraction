#include "Pixel.h"


using namespace cv;



Pixel::Pixel()
{

}


void Pixel::init(int theRow, int theColumn, float theDistanceFromOrigin) {
    row = theRow;
    column = theColumn;
    distanceFromOrigin = theDistanceFromOrigin;
}


bool Pixel::operator>(const Pixel& pixel) const {
    return distanceFromOrigin > pixel.distanceFromOrigin;
}

