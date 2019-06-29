#pragma once

#include "Pixel.h"

class SortedArray {
public:
    SortedArray(int num_elements_required);

    ~SortedArray();

    void emplace(int row, int column, float distanceFromOrigin);

    Pixel extractMin();

    friend std::ostream& operator<< (std::ostream &os, const SortedArray& list);

private:
    /*---------------*/
    /* Attributes */
    /*---------------*/
    int num_elements;
    int num_elements_required;

    Pixel *elements;

    /*---------------*/
    /* Methods */
    /*---------------*/
    bool pointIsAlreadyThere(int row, int column, int &index);
    void insert_element(int row, int column, float distanceFromOrigin);
    void delete_element(int index);
};
