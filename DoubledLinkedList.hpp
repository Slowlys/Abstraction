#pragma once

#include "Pixel.hpp"


class DoubledLinkedList {
public:
    DoubledLinkedList(unsigned int num_nodes_required);

    ~DoubledLinkedList();

    void emplace(const cv::Point coordinate, float distanceFromOrigin);
    void emplace_nonEmpty(const cv::Point coordinate, float distanceFromOrigin);

    Pixel extractMin();

    friend std::ostream& operator<< (std::ostream &os, const DoubledLinkedList& list);    

private:
    using Node = struct _node;

    /*---------------*/
    /* Attributes */
    /*---------------*/
    Node *smallest;

    unsigned int num_nodes;
    unsigned int num_nodes_required;

    /*---------------*/
    /* Methods */
    /*---------------*/

    void insert_node(Node *new_node);
};
