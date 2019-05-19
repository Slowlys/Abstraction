#pragma once

#include "Pixel.hpp"


class UnrolledLinkedList {
public:
    UnrolledLinkedList(uint num_elements_required);

    ~UnrolledLinkedList();

    void emplace(int row, int column, float distanceFromOrigin);
    void emplace_nonEmpty(int row, int column, float distanceFromOrigin);

    Pixel extractMin();

    friend std::ostream& operator<< (std::ostream &os, const UnrolledLinkedList& list);

private:
    using Node = struct _node;

    /*---------------*/
    /* Attributes */
    /*---------------*/
    Node *smallest;

    uint num_elements;
    uint num_elements_required;

    /*---------------*/
    /* Methods */
    /*---------------*/

    void insert_element(int row, int column, float distanceFromOrigin);
    bool pointIsAlreadyThere(int row, int column, Node **founded, int &index_founded);
};
