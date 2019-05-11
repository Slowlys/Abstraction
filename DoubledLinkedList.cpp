#include "DoubledLinkedList.hpp"


using namespace cv;

/* Definitions */

struct _node
{
    _node (const Point coordinate, const float distanceFromOrigin) :
        element{coordinate, distanceFromOrigin}
    {}

    _node *prev, *next;
    Pixel element;
};


//--------------------------------------------------

DoubledLinkedList::DoubledLinkedList(unsigned int num_nodes_required) :
    smallest{nullptr},
    num_nodes{0},
    num_nodes_required{num_nodes_required}
{

}


DoubledLinkedList::~DoubledLinkedList() {
    if (num_nodes > 0) {
        Node *current = smallest->next;

        while (current != smallest) {
            Node *currentNext = current->next;
            delete current;
            current = currentNext;
        }

        delete smallest;
    }
}


Pixel DoubledLinkedList::extractMin() {
    Pixel minimum = smallest->element;   

    Node *prev_smallest = smallest;
    smallest = smallest->next;
    
    prev_smallest->prev->next = smallest;
    smallest->prev = prev_smallest->prev;
   
    delete prev_smallest;

    --num_nodes;
    --num_nodes_required;

    return minimum;
}


void DoubledLinkedList::emplace(const cv::Point coordinate, float distanceFromOrigin) {
    if (smallest == nullptr) {
        smallest = new Node{coordinate, distanceFromOrigin};
        smallest->next = smallest->prev = smallest;
        ++num_nodes;
    } else {
        emplace_nonEmpty(coordinate, distanceFromOrigin);
    }
}


void DoubledLinkedList::emplace_nonEmpty(const cv::Point coordinate, float distanceFromOrigin) {

    // Find out if the point is already there
    Node *current = smallest;
    bool point_alreadyThere = false;

    do {
        current = current->next;
        point_alreadyThere = (current->element.getCoordinate() == coordinate);
    } while ( (current != smallest) && (!point_alreadyThere) );

    if (point_alreadyThere) {
        if (current->element.getDistanceFromOrigin() > distanceFromOrigin) {
            Node *new_node = new Node{coordinate, distanceFromOrigin};
            insert_node(new_node);

            // Suppress duplicated node
            current->prev->next = current->next;
            current->next->prev = current->prev;
            delete current;
        }
    } else {
        Node *new_node = new Node{coordinate, distanceFromOrigin};

        if (num_nodes >= num_nodes_required) {
            Node *max_node = smallest->prev;
            if (max_node->element > new_node->element) {
                insert_node(new_node);

                // Supress max node
                max_node->prev->next = max_node->next;
                max_node->next->prev = max_node->prev;
                delete max_node;
            }
        } else {
            insert_node(new_node);
            ++num_nodes;
        }
    }
}


void DoubledLinkedList::insert_node(Node *new_node) {
    Node *current = smallest;
    do {
        current = current->prev;
    } while ((current != smallest) && (current->element > new_node->element));

    if (smallest->element > new_node->element) {
        current = smallest->prev;
        smallest = new_node;
    }

    new_node->prev = current;
    new_node->next = current->next;

    current->next->prev = new_node;
    current->next = new_node;
}


std::ostream& operator<< (std::ostream &os, const DoubledLinkedList& list) {
    DoubledLinkedList::Node *current = list.smallest;

    do {
        os << current->element.getDistanceFromOrigin() << " ";
        current = current->next;
    } while (current != list.smallest);

    return os;
}
