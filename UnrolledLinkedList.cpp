#include "UnrolledLinkedList.hpp"


using namespace cv;

/* Definitions */


// TODO : Parcourir les tableaux avec des pointeurs
struct _node
{
    _node () :
        num_elements{0}
    {

    }


    bool startGreatThan (float distanceFromOrigin) const {
        return array[0].getDistanceFromOrigin() > distanceFromOrigin;
    }


    bool endGreatThan (float distanceFromOrigin) const {
        return array[num_elements - 1].getDistanceFromOrigin() > distanceFromOrigin;
    }


    bool coordinate_found(int row, int column, int &index) {
        bool coordinate_wasFound = false;

        for (index = 0; index < num_elements && !coordinate_wasFound; ++index) {
            coordinate_wasFound = array[index].asSameCoordinates(row, column);
        }
        --index;

        return coordinate_wasFound;
    }


    void insert(int row, int column, float distanceFromOrigin) {
        if (num_elements == size_array) {
            split();

            if (endGreatThan(distanceFromOrigin)) {
                insert_nonFull(row, column, distanceFromOrigin);
            } else {
                this->next->insert_nonFull(row, column, distanceFromOrigin);
            }
        } else {
            insert_nonFull(row, column, distanceFromOrigin);
        }
    }


    void insert_nonFull(int row, int column, float distanceFromOrigin) {
        int index_newElement = 0;
        bool insert_after = false;

        for (int i = num_elements - 1; i >= 0 && !insert_after; --i) {
            insert_after = distanceFromOrigin > array[i].getDistanceFromOrigin();
            if (!insert_after) {
                array[i + 1] = std::move(array[i]);
            } else {
                index_newElement = i + 1;
            }
        }

        array[index_newElement].init(row, column, distanceFromOrigin);
        ++num_elements;
    }


    void split() {
        static const uint half_sizeArray = size_array / 2;
        int new_index = 0;
        struct _node *new_node = new struct _node();

        for (uint index_toBeMoved = half_sizeArray; index_toBeMoved < size_array; ++index_toBeMoved) {
            new_node->array[new_index++] = std::move(array[index_toBeMoved]);
        }

        new_node->num_elements = new_index;
        this->num_elements = half_sizeArray;

        new_node->prev = this;
        new_node->next = this->next;

        this->next->prev = new_node;
        this->next = new_node;
    }


    void delete_element(int index, struct _node *head) {
        static const int half_sizeArray = size_array / 2;

        crush(index);

        if ( (num_elements == 0) && (this != head) ) {
            remove();
        } else {
            if ( (num_elements < half_sizeArray) && (next != head) ) {
                if (next->num_elements - 1 < half_sizeArray) {
                    // Merge this node with the next node
                    for (int i = 0; i < next->num_elements; ++i) {
                        array[num_elements++] = std::move(next->array[i]);
                    }
                    next->remove();
                } else {
                    // Move first element of the next node at the end of this node
                    array[num_elements++] = next->array[0];
                    next->crush(0);
                    if (next->num_elements == 0) {
                        remove();
                    }
                }
            }
        }
    }


    void crush(int index) {
        for (int i = index; i < num_elements - 1; ++i) {
            array[i] = std::move(array[i + 1]);
        }
        num_elements--;
    }


    void remove () {
        prev->next = this->next;
        next->prev = this->prev;
        delete this;
    }

    _node *prev, *next;
    const static int size_array = 50;
    int num_elements;
    Pixel array[size_array];
};


//--------------------------------------------------

UnrolledLinkedList::UnrolledLinkedList(uint num_elements_required) :
    smallest{nullptr},
    num_elements{0},
    num_elements_required{num_elements_required}
{

}


UnrolledLinkedList::~UnrolledLinkedList() {
    if (num_elements > 0) {
        Node *current = smallest->next;

        while (current != smallest) {
            Node *currentNext = current->next;
            delete current;
            current = currentNext;
        }

        delete smallest;
    }
}


Pixel UnrolledLinkedList::extractMin() {
    Pixel minimum = smallest->array[0];

    smallest->delete_element(0, smallest);

    --num_elements;
    --num_elements_required;

    return minimum;
}


void UnrolledLinkedList::emplace(int row, int column, float distanceFromOrigin) {
    if (smallest == nullptr) {
        smallest = new Node{};
        smallest->insert(row, column, distanceFromOrigin);
        smallest->next = smallest->prev = smallest;
        ++num_elements;
    } else {
        emplace_nonEmpty(row, column, distanceFromOrigin);
    }
}


void UnrolledLinkedList::emplace_nonEmpty(int row, int column, float distanceFromOrigin) {
    Node *founded = nullptr;
    int index_founded;
    const bool pointAlreadyThere = pointIsAlreadyThere(row, column, &founded, index_founded);

    if (pointAlreadyThere) {
        if (founded->array[index_founded].getDistanceFromOrigin() > distanceFromOrigin) {
            founded->delete_element(index_founded, smallest);
            insert_element(row, column, distanceFromOrigin);
        }
    } else {
        if (num_elements >= num_elements_required) {
            Node *max_node = smallest->prev;
            if (max_node->endGreatThan(distanceFromOrigin)) {
                max_node->delete_element(max_node->num_elements - 1, smallest);
                insert_element(row, column, distanceFromOrigin);
            }
        } else {
            insert_element(row, column, distanceFromOrigin);
            ++num_elements;
        }
   }
}


bool UnrolledLinkedList::pointIsAlreadyThere (int row, int column, Node **founded, int &index_founded) {
    *founded = smallest;
    bool point_alreadyThere = false;

    do {
        *founded = (*founded)->next;
        point_alreadyThere = (*founded)->coordinate_found(row, column, index_founded);
    } while ( (*founded != smallest) && (!point_alreadyThere) );

    return point_alreadyThere;
}


void UnrolledLinkedList::insert_element(int row, int column, float distanceFromOrigin) {
    Node *current = smallest;

    do {
        current = current->prev;
    } while ((current != smallest) && (current->startGreatThan(distanceFromOrigin)));

    current->insert(row, column, distanceFromOrigin);
}


std::ostream& operator<< (std::ostream &os, const UnrolledLinkedList& list) {
    UnrolledLinkedList::Node *current = list.smallest;

    do {
        for (int i = 0; i < current->num_elements; ++i) {
            os << current->array[i].getDistanceFromOrigin() << " ";
        }

        for (int i = 0; i < current->size_array - current->num_elements; ++i) {
            os << " * ";
        }

        os << " -> ";
        current = current->next;
    } while (current != list.smallest);

    return os;
}
