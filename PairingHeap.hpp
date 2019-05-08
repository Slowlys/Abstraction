#pragma once

#include "Pixel.hpp"


// TODO : Template
class PairingHeap {
public:
    PairingHeap();

    ~PairingHeap();

    void emplace(const cv::Point coordinate, float distanceFromOrigin);
    void emplace_nonEmpty(const cv::Point coordinate, float distanceFromOrigin);

    Pixel extractMin();

private:
    using Node = struct _node;

    /*---------------*/
    /* Attributes */
    /*---------------*/
    Node *smallest;

    /*---------------*/
    /* Functions */
    /*---------------*/

    Node* merge(Node *rootA, Node *rootB);

    void merge_nonEmpty(Node *smaller, Node *other);

    void delete_heap(Node *root);

    void insert_nonEmpty(Node *node);
};
