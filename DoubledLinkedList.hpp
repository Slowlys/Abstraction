#pragma once

#include "Pixel.hpp"


class DoubledLinkedList {
public:
    DoubledLinkedList(uint num_nodes_required);

    ~DoubledLinkedList();

    void emplace(int row, int column, float distanceFromOrigin);
    void emplace_nonEmpty(int row, int column, float distanceFromOrigin);

    Pixel extractMin();

    friend std::ostream& operator<< (std::ostream &os, const DoubledLinkedList& list);    

private:
    using Node = struct _node;

    /*---------------*/
    /* Attributes */
    /*---------------*/
    Node *smallest;

    uint num_nodes;
    uint num_nodes_required;

    /*---------------*/
    /* Methods */
    /*---------------*/

    void insert_node(Node *new_node);
    void delete_node(Node *node);
    bool pointIsAlreadyThere(int row, int column, Node **founded);
};
