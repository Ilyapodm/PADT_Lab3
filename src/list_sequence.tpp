#pragma once

#include "ienumerator.hpp"
#include "linked_list.hpp"
#include "list_sequence.hpp"
#include "sequence.hpp"
#include <stdexcept>

/*******************************************************************
 * constructors
 *******************************************************************/

template <typename T>
ListSequence<T>::ListSequence() {
    list = new LinkedList<T>();
}

template <typename T>
ListSequence<T>::ListSequence(T *items, int size) {
    list = new LinkedList<T>(items, size);
}

template <typename T>
ListSequence<T>::ListSequence(const LinkedList<T> &list) {
    this->list = new LinkedList<T>(list);
}

template <typename T>
ListSequence<T>::ListSequence(const ListSequence<T> &other) {
    list = new LinkedList<T>(*other.list);
}

/*******************************************************************
 * getters
 *******************************************************************/

template <typename T>
const T& ListSequence<T>::get_first() const {
    return list->get_first();
}

template <typename T>
const T& ListSequence<T>::get_last() const {
    return list->get_last();
}

template <typename T>
const T& ListSequence<T>::get(int index) const {
    return list->get(index);
}

template <typename T>
int ListSequence<T>::get_size() const {
    return list->get_length();
}

/*******************************************************************
 * operations
 *******************************************************************/

template <typename T>
Sequence<T>* ListSequence<T>::append(const T &item) {

    ListSequence<T> *inst = this->instance();

    try {
        inst->list->append(item);
    } catch (...) {
        // mutable can fall only in "append". In this case "this" is unchanged
        // immutable can fall during creation (nothing to do), and during append:
        // in this case we lose inst, so need to delete
        if (inst != this) 
            delete inst;  
        throw;
    }

    return inst;
}

template <typename T>
Sequence<T>* ListSequence<T>::prepend(const T &item) {
    ListSequence<T> *inst = this->instance();

    try {
        inst->list->prepend(item);
    } catch (...) {
        // mutable can fall only in "prepend". In this case "this" is unchanged
        // immutable can fall during creation (nothing to do), and during prepend:
        // in this case we lose inst, so need to delete
        if (inst != this) 
            delete inst;  
        throw;
    }

    return inst;
}

template <typename T>
Sequence<T>* ListSequence<T>::insert_at(const T &item, int index) {
    ListSequence<T> *inst = this->instance();

    try {
        inst->list->insert_at(item, index);
    } catch (...) {
        // mutable can fall only in "insert_at". In this case "this" is unchanged
        // immutable can fall during creation (nothing to do), and during insert_at:
        // in this case we lose inst, so need to delete
        if (inst != this) 
            delete inst;  
        throw;
    }

    return inst;
}

template <typename T>
Sequence<T>* ListSequence<T>::set(const T &item, int index) {
    if (index < 0 || index >= get_size())
        throw std::out_of_range("set: index out of range");

    ListSequence<T> *inst = this->instance();

    try {
        // T operator= can fail: for mutable have to delete too
        inst->list->set(index, item);
    } catch (...) {
        if (this != inst)
            delete inst;
        throw;
    }
    
    return inst;
}

template <typename T>
Sequence<T>* ListSequence<T>::remove_at(int index) {
    if (index < 0 || index >= get_size())
        throw std::out_of_range("remove_at: index out of range");

    ListSequence<T> *inst = this->instance();

    try {
    inst->list->remove_at(index);
    } catch (...) {
        if (this != inst) 
            delete inst;
        throw;
    }

    return inst;
}

// type of sublist (mutable or immutable) is the type of "this"
template <typename T>
Sequence<T>* ListSequence<T>::get_subsequence(int start_index, int end_index) const {
    if (start_index < 0 || end_index < start_index || end_index >= this->get_size()) 
        throw std::out_of_range("get_subsequence: index out of range");

    ListSequence<T> *result = this->empty_sequence();

    // HOW IS IT WORKING:
    // result's already had a pointer to "list" (empty). if we use '=' 
    // without deletion empty "list" will be lost (memory leak).
    // HOW COULD (or couldn't) IT WORK 
    // one way: use operator '=' for LinkedList. But then temp list 
    // from get_sublist will be lost (memory leak)
    // another way: do not use get_sublist(), and just work with 
    // result's empty "list" by appending the elements (just like in array sequence) 
    try{
        delete result->list; 
        result->list = this->list->get_sublist(start_index, end_index);  // working with pointers, not lists
    } catch(...) {
        result->list = nullptr;  // don't have to delete it again with "delete result"
        delete result;
        throw;
    }

    return result;
}

template <typename T>
Sequence<T>* ListSequence<T>::concat(const Sequence<T> &other) const {
    ListSequence<T> *result = this->empty_sequence();

    IEnumerator<T> *this_iter = this->get_enumerator();  // use enumerator to optimize get
    IEnumerator<T> *other_iter = other.get_enumerator();
    try {
        while (this_iter->move_next()) {
            result->list->append(this_iter->get_current());
        }

        while (other_iter->move_next()) {
            result->list->append(other_iter->get_current());
        }
    } catch (...) {
        delete this_iter;
        delete other_iter;
        delete result;
        throw;
    }

    delete this_iter;
    delete other_iter;

    return result;
}

/*******************************************************************
 * map, where, reduce
 *******************************************************************/

template <typename T>
Sequence<T>* ListSequence<T>::map(T (*mapper)(const T &element)) {
    ListSequence<T> *inst = this->instance();
    
    try {
        inst->list->transform(mapper);
    } catch (...) {
        if (inst != this)
            delete inst;
        throw;
    }  

    return inst;
}

template <typename T>
Sequence<T>* ListSequence<T>::where(bool (*predicate)(const T &element)) {
    // "set_current" doesn't have the needed access to the list:
    // it can't delete nodes, change the structure of the list
    // so "where" delegates it to "filter" in "linked_list", to do it quick 
    ListSequence<T> *inst = this->instance();
    
    try {
        inst->list->filter(predicate);
    } catch (...) {
        if (inst != this)
            delete inst;
        throw;
    }  

    return inst;
}

template <typename T>
T ListSequence<T>::reduce(T (*reduce_func)(const T &first_element, const T &second_element), const T &start_element) {
    T result = start_element;

    IEnumerator<T>* this_iter = get_enumerator();  // use enumerator not to repeat "get" loop

    while(this_iter->move_next()) {
        result = reduce_func(this_iter->get_current(), result);  // usually start_element is the 1 arg
    }

    delete this_iter;

    return result;
}

template <typename T>
Sequence<T>* ListSequence<T>::slice(int index, int count, const Sequence<T> &seq) {
    // if "count" go out of the edge, silently but it till this edge
    if (count < 0)
        throw::std::invalid_argument("slice: count cannot be negetive");
    if (index < 0)
        index = get_size() + index;
    if (index < 0 || index >= get_size()) 
        throw std::out_of_range("slice: Index out of range");
        
    //         left part      deleted        right part
    // this: [0 .. index) [index .. end) [end .. get_size())

    //       left part        inserted          right part
    // inst: [0 .. index) [ seq.get_size() ] [end .. get_size())
    int end = get_size() < index + count ? get_size() : index + count;  // the next element after replaced elements

    ListSequence<T> *inst = this->instance();

    int new_size = index + seq.get_size() + (get_size() - end);

    LinkedList<T> *new_list = new LinkedList<T>();

    try {
        for (int i = 0; i < index; i++) {
            new_list->append(get(i));  // copy left side
        }

        for (int i = 0; i < seq.get_size(); i++) {
            new_list->append(seq.get(i));  // copy from seq
        }

        for (int i = end; i < get_size(); i++) {
            new_list->append(get(i));
        }
    } catch (...) {
        delete new_list;
        if (inst != this)
            delete inst;
        throw;
    }

    delete inst->list;
    inst->list = new_list;
    return inst;
}