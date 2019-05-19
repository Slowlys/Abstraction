  #include "DoubledLinkedList.hpp"


using namespace cv;

/* Definitions */

struct _node
{
    _node (int row, int column, const float distanceFromOrigin) :
        element{row, column, distanceFromOrigin}
    {}

    _node *prev, *next;
    Pixel element;
};


//--------------------------------------------------

DoubledLinkedList::DoubledLinkedList(uint num_nodes_required) :
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


void DoubledLinkedList::emplace(int row, int column, float distanceFromOrigin) {
    if (smallest == nullptr) {
        smallest = new Node{row, column, distanceFromOrigin};
        smallest->next = smallest->prev = smallest;
        ++num_nodes;
    } else {
        emplace_nonEmpty(row, column, distanceFromOrigin);
    }
}


void DoubledLinkedList::emplace_nonEmpty(int row, int column, float distanceFromOrigin) {
    Node *found = nullptr;
    const bool pointAlreadyThere = pointIsAlreadyThere(row, column, &found);

    if (pointAlreadyThere) {
        if (found->element.getDistanceFromOrigin() > distanceFromOrigin) {
            Node *new_node = new Node{row, column, distanceFromOrigin};
            insert_node(new_node);          
            delete_node(found);
        }
    } else {
        Node *new_node = new Node{row, column, distanceFromOrigin};

        if (num_nodes >= num_nodes_required) {
            Node *max_node = smallest->prev;
            if (max_node->element > new_node->element) {
                insert_node(new_node);
                delete_node(max_node);
            } else {
                delete new_node;
            }
        } else {
            insert_node(new_node);
            ++num_nodes;
        }
    }
}


bool DoubledLinkedList::pointIsAlreadyThere (int row, int column, Node **founded) {
    *founded = smallest;
    bool point_alreadyThere = false;

    do {
        *founded = (*founded)->next;
        point_alreadyThere = (*founded)->element.asSameCoordinates(row, column);
    } while ( (*founded != smallest) && (!point_alreadyThere) );

    return point_alreadyThere;
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


void DoubledLinkedList::delete_node(Node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    delete node;
}


std::ostream& operator<< (std::ostream &os, const DoubledLinkedList& list) {
    DoubledLinkedList::Node *current = list.smallest;

    do {
        os << current->element.getDistanceFromOrigin() << " ";
        current = current->next;
    } while (current != list.smallest);

    return os;
}
