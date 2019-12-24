#include "SortedArray.h"


SortedArray::SortedArray(int num_elements_required) :
    num_elements{0},
    num_elements_required{num_elements_required},
    elements{new Pixel[num_elements_required - (num_elements_required / 5)]}
{

}


SortedArray::~SortedArray() {
    delete [] elements;
}


Pixel SortedArray::extractMin() {
    Pixel minimum = std::move(elements[0]);

    delete_element(0);
    --num_elements_required;

//    std::cout << "Extract : " << *this << std::endl;
//    cv::waitKey(100);

    return minimum;
}


void SortedArray::emplace (int row, int column, float distanceFromOrigin) {
    int index_duplicate;
    const bool duplicate = pointIsAlreadyThere(row, column, index_duplicate);

    if (duplicate) {
        if (elements[index_duplicate].getDistanceFromOrigin() > distanceFromOrigin) {
            delete_element(index_duplicate);
            insert_element(row, column,distanceFromOrigin);
        }

        //std::cout << "** Duplicate ** " << std::endl;
    } else {
        if (num_elements >= num_elements_required) {
            if (elements[num_elements - 1].getDistanceFromOrigin() > distanceFromOrigin) {
                --num_elements;
                insert_element(row, column,distanceFromOrigin);
            }
        } else {
            insert_element(row, column,distanceFromOrigin);
        }
    }

//    std::cout << "Insert : " << *this << std::endl;
//    cv::waitKey(100);
}


bool SortedArray::pointIsAlreadyThere(int row, int column, int &index) {
    bool wasFounded = false;

    for (index = 0; index < num_elements && !wasFounded; ++index) {
        wasFounded = elements[index].asSameCoordinates(row, column);
    }

    --index;
    return wasFounded;
}


void SortedArray::insert_element(int row, int column, float distanceFromOrigin) {
    int index_newElement = 0;
    bool insert_after = false;

    for (int i = num_elements - 1; i >= 0 && !insert_after; --i) {
        insert_after = (distanceFromOrigin > elements[i].getDistanceFromOrigin());
        if (insert_after) {
            index_newElement = i + 1;
        } else {
            elements[i + 1] = std::move(elements[i]);
        }
    }

    elements[index_newElement].init(row, column, distanceFromOrigin);
    ++num_elements;
}


void SortedArray::delete_element(int index) {
    std::move(&elements[index + 1], &elements[num_elements], &elements[index]);
    --num_elements;
}


std::ostream& operator<< (std::ostream &os, const SortedArray& list) {
    for (int i = 0; i < list.num_elements; ++i) {
        os << list.elements[i].getDistanceFromOrigin() << " ";
    }

    return os;
}
