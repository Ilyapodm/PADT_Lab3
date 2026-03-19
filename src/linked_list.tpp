#pragma once

#include "linked_list.hpp"
#include <stdexcept>

/*******************************************************************
 * constructors and operator=
 *******************************************************************/

template <typename T>
LinkedList<T>::LinkedList() : head{nullptr}, tail{nullptr}, length{0} {}

template <typename T>
LinkedList<T>::LinkedList(T *items, int size) : head{nullptr}, tail{nullptr}, length{size} {
    if (size < 0)
        throw std::invalid_argument("LinkedList: length cannot be negative");

    if (items == nullptr && size > 0)
        throw std::invalid_argument("LinkedList: items is nullptr");

    if (size == 0) 
        return;

    try {
        // create first Node (head and tail at the same time)
        head = new Node{.data = items[0], .next = nullptr};
        tail = head;

        for (int i = 1; i < size; i++) {
            // can fail on 2, 3... iteration
            tail->next = new Node{.data = items[i], .next = nullptr};  // create new Node (tail's next)
            tail = tail->next;  // move tail to just created Node
        }
    } catch(...) {
        Node *current_node = head;
        while (current_node != nullptr) {
            Node *temp_node = current_node;
            current_node = current_node->next;
            delete temp_node;
        }
        head = tail = nullptr;
        throw;
    }
}

template <typename T>
LinkedList<T>::LinkedList(const LinkedList<T> &other) : head{nullptr}, tail{nullptr}, length{0} {
    if (other.length == 0) 
        return;

    // create first Node (head and tail at the same time)
    try {
        Node *src_node = other.head;
        while (src_node != nullptr) {
            append(src_node->data);
            src_node = src_node->next;
        }
    } catch (...) {
        Node *current_node = head;
        while (current_node != nullptr) {
            Node *temp_node = current_node;
            current_node = current_node->next;
            delete temp_node;
        }

        head = tail = nullptr;
        throw;
    }
}

template <typename T>
LinkedList<T>::~LinkedList() {
    Node *current_node = head;

    while (current_node != nullptr) {
        Node* temp = current_node;
        current_node = current_node->next;
        delete temp;
    }
}

template <class T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& other) {
    if (this == &other) return *this;

    // create new list first
    Node* new_head = nullptr;
    Node* new_tail = nullptr;
    Node* src = other.head;
    try {
        while (src != nullptr) {
            Node* node = new Node{.data = src->data, .next = nullptr};
            if (!new_head) 
                new_head = new_tail = node;
            else { 
                new_tail->next = node;
                new_tail = node;
            }
            src = src->next;
        }
    } catch (...) {
        // clear the broken list
        while (new_head) {
            Node* temp = new_head;
            new_head = new_head->next;
            delete temp;
        }
        throw;
    }

    // after all delete the old one
    Node* curr = head;
    while (curr) { Node* t = curr; curr = curr->next; delete t; }

    head = new_head; tail = new_tail; length = other.length;
    return *this;
}

/*******************************************************************
 * getters
 *******************************************************************/

template <typename T>
const T& LinkedList<T>::get_first() const {
    if (head == nullptr) 
        throw std::out_of_range("get_first: The list is empty");
    

    return head->data;
}

template <typename T>
const T& LinkedList<T>::get_last() const {
    if (tail == nullptr) 
        throw std::out_of_range("get_last: The list is empty");
    

    return tail->data;
}

template <typename T>
const T& LinkedList<T>::get(int index) const {
    if (index < 0 || index >= length)
        throw std::out_of_range("get: Index out of range");

    Node *current_node = head;

    for (int i = 0; i < index; i++) {
        current_node = current_node->next;
    }

    return current_node->data;
}

template <typename T>
int LinkedList<T>::get_length() const {
    return length;
}

/*******************************************************************
 * operations
 *******************************************************************/

template <typename T>
void LinkedList<T>::append(const T& item) {
    
    Node *new_node = new Node{.data = item, .next = nullptr};

    if (head == nullptr) {
        head = new_node;
        tail = head;
    }
    else {
        tail->next = new_node;
        tail = tail->next;
    }

    length++;
}

template <typename T>
void LinkedList<T>::prepend(const T& item) {
    Node *new_node = new Node{.data = item, .next = nullptr};

    if (head == nullptr) {
        head = new_node;
        tail = head;
    }
    else {
        new_node->next = head;
        head = new_node;
    }

    length++;
}

template <typename T>
void LinkedList<T>::insert_at(const T& item, int index) {
    // inserting at the end is available (the same as append)
    if (index < 0 || index > length) {
        throw std::out_of_range("insert_at: Index out of range");
    }

    if (index == 0) {
        this->prepend(item);
        return;
    }

    if (index == length) {
        this->append(item);
        return;
    }

    Node *new_node = new Node{.data = item, .next = nullptr};
    Node *current_node = head;

    for (int i = 0; i < index - 1; i++) {
        current_node = current_node->next;
    }

    Node *next_node = current_node->next;
    current_node->next = new_node;
    new_node->next = next_node;

    length++;
}

template <typename T>
void LinkedList<T>::set(int index, const T& item) {
    if (index >= this->length || index < 0) {
        throw std::out_of_range("set: Index out of range");
    }
    
    Node *current_node = head;
    for (int i = 0; i < index; i++) {
        current_node = current_node->next;
    }

    current_node->data = item;
}

template <typename T>
T LinkedList<T>::remove_at(int index) {
    if (index < 0 || index >= get_length())
        throw std::out_of_range("remove_at: Index out of range");

    T value;
    Node *node_to_delete;

    if (index == 0) {
        node_to_delete = head;
        value = head->data;
        head = head->next;
        if (head == nullptr)
            tail = nullptr;
    }
    else {
        Node *previous_node = head;
        // index - 1 because we iterate not by current, but by previous
        for (int i = 0; i < index - 1; i++) {
            previous_node = previous_node->next;
        }

        node_to_delete = previous_node->next;
        value = node_to_delete->data;
        previous_node->next = node_to_delete->next;  // changes the links
        if (node_to_delete->next == nullptr) 
            tail = previous_node;
    }    

    length--;
    delete node_to_delete;
    return value;
}

template <typename T>
LinkedList<T>* LinkedList<T>::get_sublist(int start_index, int end_index) const {
    if (head == nullptr)
        throw std::out_of_range("get_sublist: The list is empty");
    if (start_index < 0 || end_index >= length)
        throw std::out_of_range("get_sublist: Index out of range");
    if (start_index > end_index)
        throw std::out_of_range("get_sublist: Start index can't be greater than end index");

    LinkedList<T>* result = new LinkedList<T>();

    Node* current_node = head;
    for (int i = 0; i < start_index; i++)
        current_node = current_node->next;

    // append can fail on the 2, 3... iteration, list will be broken
    try {
        for (int i = start_index; i <= end_index; i++) {
            result->append(current_node->data);
            current_node = current_node->next;
        }
    } catch (...) {
        delete result;
        throw;
    }

    return result;
}

template <typename T>
LinkedList<T>* LinkedList<T>::concat(const LinkedList<T> *other) const {
    if (other == nullptr) {
        throw std::invalid_argument("concat: Other list is nullptr");
    }

    LinkedList<T> *result = new LinkedList<T>();
    
    Node *current_node = head;

    try {
        for (int i = 0; i < length; i++) {
            result->append(current_node->data);
            current_node = current_node->next;
        }

        current_node = other->head;

        for (int i = 0; i < other->length; i++) {
        result->append(current_node->data);
        current_node = current_node->next;
        }
    } catch (...){
        delete result;
        throw;
    }

    return result;
}

/*******************************************************************
 * heigher-order functions (map, where...)
 *******************************************************************/

template <typename T>
void LinkedList<T>::filter(bool (*predicate)(const T&)) {
    Node *previous_node = nullptr;
    Node *current_node = head;

    while (current_node != nullptr) {
        Node *next = current_node->next;
        if (predicate(current_node->data)) {
            previous_node = current_node;
        } 
        else {
            if (previous_node) 
                previous_node->next = next;
            else 
                head = next;  
            delete current_node;
            length--;
        }
        current_node = next;
    }
    tail = previous_node;  
}

template <typename T>
void LinkedList<T>::transform(T (*mapper)(const T&)) {
    Node* current = head;
    while (current != nullptr) {
        current->data = mapper(current->data);
        current = current->next;
    }
}

/*******************************************************************
 * Enumerator
 *******************************************************************/

template <typename T>
IEnumerator<T>* LinkedList<T>::get_enumerator() const {
        return new ListEnumerator(this->head);
    }

template <typename T>
bool LinkedList<T>::ListEnumerator::move_next() {
    if (! started) {
        current_node = head;
        started = true;
    }
    else if (current_node != nullptr)  // fallback if user decides use after false return
        current_node = current_node->next;

    return current_node != nullptr;
}

template <typename T>
const T& LinkedList<T>::ListEnumerator::get_current() const {
    if (! started || current_node == nullptr)
        throw std::out_of_range("get_current(): called in invalid state");
    return current_node->data;
}

// template <typename T>
// void LinkedList<T>::ListEnumerator::set_current(const T &value) {
//     if (index < 0 || index >= linked_list->get_length())
//         throw std::out_of_range("set_current(): called in invalid state");
//     current_node->data = value;
// }

template <typename T>
void LinkedList<T>::ListEnumerator::reset() {
    started = false;
    current_node = nullptr;
}