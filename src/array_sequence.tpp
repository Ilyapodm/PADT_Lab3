#pragma once

#include "array_sequence.hpp"
#include "dynamic_array.hpp"
#include "ienumerator.hpp"
#include "sequence.hpp"
#include <stdexcept>

/*******************************************************************
 * constructors
 *******************************************************************/

template <typename T>
ArraySequence<T>::ArraySequence() {
    array = new DynamicArray<T>();
}

template <typename T>
ArraySequence<T>::ArraySequence(T *items, int size) {
    array = new DynamicArray<T>(items, size); 
}

template <typename T>
ArraySequence<T>::ArraySequence(const DynamicArray<T> &array) {
    this->array = new DynamicArray<T>(array);
}

template <typename T>
ArraySequence<T>::ArraySequence(const ArraySequence<T> &other) {
    array = new DynamicArray<T>(*other.array);
}

/*******************************************************************
 * getters
 *******************************************************************/


template <typename T>
const T& ArraySequence<T>::get_first() const {
    return array->get(0);
}

template <typename T>
const T& ArraySequence<T>::get_last() const {
    return array->get(array->get_size() - 1);
}

template <typename T>
const T& ArraySequence<T>::get(int index) const {
    return array->get(index);
}

template <typename T>
int ArraySequence<T>::get_size() const {
    return array->get_size();
}

/*******************************************************************
 * operations
 *******************************************************************/

template <typename T>
Sequence<T>* ArraySequence<T>::append(const T &item) {
    ArraySequence<T> *inst = this->instance();  // make a copy

    try {
        inst->array->resize(inst->array->get_size() + 1);  
        inst->array->set(inst->array->get_size() - 1,  item);  // set at size-1 because size is increased on previous string   
    } catch (...) {
        if (inst != this)  // this (if resize falls) is in normal condition, don't have to delete
            delete inst;
        throw;
    }
    
    return inst;
}

template <typename T>
Sequence<T>* ArraySequence<T>::prepend(const T &item) {
    ArraySequence<T> *inst = this->instance();

    try{
        inst->array->resize(inst->array->get_size() + 1);
        
        for (int dst = inst->array->get_size() - 1; dst > 0; dst--) {
            // T operator= can fail: for mutable have to delete too
            inst->array->set(dst, inst->array->get(dst - 1));
        }
        inst->array->set(0,  item);
    } catch (...) {
        delete inst;
        throw;
    }

    return inst;
}

template <typename T>
Sequence<T>* ArraySequence<T>::insert_at(const T &item, int index) {
    if (index < 0 || index > array->get_size())
        throw std::out_of_range("insert_at: index out of range");

    ArraySequence<T> *inst = this->instance();

    try {
        inst->array->resize(inst->array->get_size() + 1);
        // T operator= can fail: for mutable have to delete too
        for (int dst = inst->get_size() - 1; dst > index; dst--) {
            inst->array->set(dst, inst->array->get(dst - 1));
        }

        inst->array->set(index, item);
    } catch (...) {
        delete inst;
        throw;
    }
    
    return inst;
}

template <typename T>
Sequence<T>* ArraySequence<T>::set(const T &item, int index) {
    if (index < 0 || index >= get_size())
        throw std::out_of_range("set: index out of range");

    ArraySequence<T> *inst = this->instance();

    try {
        // T operator= can fail: for mutable have to delete too
        inst->array->set(index, item);
    } catch (...) {
        if (this != inst)
            delete inst;
        throw;
    }
    
    return inst;
}

template <typename T>
Sequence<T>* ArraySequence<T>::remove_at(int index) {
    if (index < 0 || index >= get_size())
        throw std::out_of_range("remove_at: index out of range");

    ArraySequence<T> *inst = this->instance();

    try {
    inst->array->remove_at(index);
    } catch (...) {
        if (this != inst) 
            delete inst;
        throw;
    }

    return inst;
}

template <typename T>
Sequence<T>* ArraySequence<T>::get_subsequence(int start_index, int end_index) const {
    if (start_index < 0 || end_index >= array->get_size() || start_index > end_index)  // cover size = 0
        throw std::out_of_range("get_subsequence: index out of range");

    // need this method because we don't know what (mutable/immutable) we are working with
    ArraySequence<T> *result = this->empty_sequence();

    try {
        int total_size = end_index - start_index + 1;  

        result->array->resize(total_size);  // resize all needed space one time

        for (int i = start_index; i <= end_index; i++){
            // do not use append because it can create another copy for immutable
            result->array->set(i - start_index, array->get(i));
        }
    } catch (...) {
        delete result;
        throw;
    }

    return result;
}

template <typename T>
Sequence<T>* ArraySequence<T>::concat(const Sequence<T> &other) const {
    ArraySequence<T> *result = this->empty_sequence();

    try {
        int total_size = this->get_size() + other.get_size();
        result->array->resize(total_size);  // resize all needed space at one time

        for (int i = 0; i < this->get_size(); i++) 
            // do not use append because it can create another copy for immutable
            result->array->set(i, array->get(i));
        
        for (int i = 0; i < other.get_size(); i++) 
            result->array->set(this->get_size() + i, other.get(i));  // have already placed get_size - 1 spaces
        
    } catch (...) {
        delete result;
        throw;
    }
    
    return result;
}

/*******************************************************************
 * map, where, reduce
 *******************************************************************/

template <typename T>
Sequence<T>* ArraySequence<T>::map(T (*mapper)(const T &element)) {
    ArraySequence<T> *inst = this->instance();

    try {
        for (int i = 0; i < inst->get_size(); i++) {
            inst->array->set(i, mapper(inst->array->get(i)));
        }
    } catch (...) {
        if (this != inst) 
            delete inst;
        throw;
    }
    

    return inst;
}

template <typename T>
Sequence<T>* ArraySequence<T>::where(bool (*predicate)(const T &element)) {
    ArraySequence<T> *inst = this->instance();

    try {
        int src = 0, dst = 0;
        for (; src < inst->get_size(); src++) {
            if (predicate(inst->get(src))) {
                inst->array->set(dst, inst->get(src));
                dst++;
            }
        }
        inst->array->resize(dst);  // size can be changed
    }catch (...) {
        if (this != inst) 
            delete inst;
        throw;
    }

    return inst;
}

template <typename T>
T ArraySequence<T>::reduce(T (*reduce_func)(const T &first_element, const T &second_element), const T &start_element) {
    T result = start_element;

    for (int i = 0; i < get_size(); i++) {
        result = reduce_func(get(i), result);  // usually start_element is the 1 arg
    }

    return result;
}

template <typename T>
Sequence<T>* ArraySequence<T>::slice(int index, int count, const Sequence<T> &seq) {
    // if "count" go out of the edge, silently but it till this edge
    if (count < 0)
        throw::std::invalid_argument("slice: count cannot be negetive");
    if (index < 0)
        index = get_size() + index;
    if (index < 0 || index >= get_size()) 
        throw std::out_of_range("slice: Index out of range");
        
    //         left part    deleted        right part
    // this: [0 .. index) [index .. end) [end .. get_size())

    //       left part        inserted          right part
    // inst: [0 .. index) [ seq.get_size() ] [end .. get_size())
    int end = get_size() < index + count ? get_size() : index + count;  // the next element after replaced elements

    ArraySequence<T> *inst = this->instance();

    int new_size = index + seq.get_size() + (get_size() - end);

    DynamicArray<T> *new_array = new DynamicArray<T>(new_size);

    try {
        int dst = 0;
        for (int i = 0; i < index; i++) {
            new_array->set(dst, get(i));  // copy left side
            dst++;
        }

        for (int i = 0; i < seq.get_size(); i++) {
            new_array->set(dst, seq.get(i));  // copy from seq
            dst++;
        }

        for (int i = end; i < get_size(); i++) {
            new_array->set(dst, get(i));
            dst++;
        }
    } catch (...) {
        delete new_array;
        if (inst != this)
            delete inst;
        throw;
    }

    delete inst->array;
    inst->array = new_array;
    return inst;
}

/*******************************************************************
 * Enumerator
 *******************************************************************/

template <typename T>
IEnumerator<T>* ArraySequence<T>::get_enumerator() const {
    return new ArrayEnumerator(this);
}

template <typename T>
bool ArraySequence<T>::ArrayEnumerator::move_next() {
    index++;
    return index < array_sequence->get_size();
}

template <typename T>
const T& ArraySequence<T>::ArrayEnumerator::get_current() const {
    if (index < 0 || index >= array_sequence->get_size())
        throw std::out_of_range("get_current(): called in invalid state");
    return array_sequence->get(index);
}

// template <typename T>
// void ArraySequence<T>::ArrayEnumerator::set_current(const T &value) {
//     if (index < 0 || index >= array_sequence->get_size())
//         throw std::out_of_range("set_current(): called in invalid state");
//     array_sequence->array->set(index, value);
// }

template <typename T>
void ArraySequence<T>::ArrayEnumerator::reset() {
    index = -1;
}