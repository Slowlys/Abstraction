#include "PairingHeap.hpp"



using namespace cv;

/* Definitions */

struct _node
{
    _node (const Point coordinate, const float distanceFromOrigin) :
        leftMostchild{nullptr},
        right{nullptr},
        element{coordinate, distanceFromOrigin}
    {}

    _node *leftMostchild;
    _node *right;
    Pixel element;
};


/* Functions */

PairingHeap::PairingHeap() :
    smallest{nullptr}
{

}


PairingHeap::~PairingHeap() {
    delete_heap(smallest);
}


void PairingHeap::delete_heap(Node *root) {
    if (root != nullptr) {
        delete_heap(root->leftMostchild);
        delete_heap(root->right);
        delete root;
    }
}


// TODO : possible empty and possible non-empty
void PairingHeap::emplace(const Point coordinate, float distanceFromOrigin) {
    Node *new_node = new Node{coordinate, distanceFromOrigin};

    if (smallest == nullptr) {
        smallest = new_node;
    } else {
        insert_nonEmpty(new_node);
    }
}


void PairingHeap::emplace_nonEmpty(const Point coordinate, float distanceFromOrigin) {
    Node *new_node = new Node{coordinate, distanceFromOrigin};
    insert_nonEmpty(new_node);
}


void PairingHeap::insert_nonEmpty(Node *node) {
    if (node->element > smallest->element) {
        merge_nonEmpty(smallest, node);
    } else {
        node->leftMostchild = smallest;
        smallest = node;
    }
}


void PairingHeap::merge_nonEmpty(Node *smaller, Node *other) {
    other->right = smaller->leftMostchild;
    smaller->leftMostchild = other;
}


PairingHeap::Node* PairingHeap::merge(Node *rootA, Node *rootB) {
    Node *new_root;

    if (rootA == nullptr) {
        rootB->right = nullptr;
        new_root = rootB;
    } else {
        rootA->right = rootB->right = nullptr;

        if (rootA->element > rootB->element) {
            merge_nonEmpty(rootB, rootA);
            new_root = rootB;            
        } else {
            merge_nonEmpty(rootA, rootB);
            new_root = rootA;
        }
    }

    return new_root;
}


// TODO : plus de pointeurs null
Pixel PairingHeap::extractMin() {
    Pixel minimum = smallest->element;

    Node *current = smallest->leftMostchild;
    delete smallest;
    smallest = nullptr;

    while (current != nullptr) {
        // If there is only one subheap left
        if (current->right == nullptr) {
            smallest = merge(smallest, current);
            current = nullptr;
        } else {
            Node *currentNext = current->right->right;
            Node *merged = merge(current, current->right);
            smallest = merge(smallest, merged);
            current = currentNext;
        }
    }

    return minimum;
}

