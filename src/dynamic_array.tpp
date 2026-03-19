#pragma once

#include "dynamic_array.hpp"
#include <stdexcept>

/*******************************************************************
 * constructors
 *******************************************************************/

template <typename T> 
DynamicArray<T>::DynamicArray() : size{0}, capacity{0} {
    data = nullptr;
}

template <typename T>
DynamicArray<T>::DynamicArray(int size) : size{size}, capacity{size} {
    if (size < 0)
        throw std::invalid_argument("DynamicArray: size cannot be negative");

    data = (size > 0) ? new T[size]{} : nullptr;  // do the initialization of items
}

template <typename T>
DynamicArray<T>::DynamicArray(T *items, int size) : size{size}, capacity{size} {
    if (size < 0)
        throw std::invalid_argument("DynamicArray: size cannot be negative");

    if (items == nullptr && size > 0)
        throw std::invalid_argument("DynamicArray: items is nullptr");

    data = new T[size]{};  // do the initialization of items

    // fallback if the '=' operator for T will fall 
    try{
        for (int i = 0; i < size; i++) {
            data[i] = items[i];
        }
    } catch (...) {
        delete []data;
        throw;
    }
    
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T> &other) : size{other.size}, capacity{other.capacity} {
    data = new T[capacity]{};  // do the initialization of items

    // fallback if the '=' operator for T will fall 
    try {
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    } catch (...) {
        delete []data;
        throw;
    }
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
    delete []data;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T> &other) {
    if (&other == this)  // no self assignment
        return *this;

    // do not need 'try', 'new' is smart, if bad_alloc it will clear everything itself
    T *new_data = new T[other.capacity]{};  // do the initialization of items

    // at first try to create new data and then only 'delete' the old
    try {
        for (int i = 0; i < other.size; i++) {
            new_data[i] = other.data[i];  // here can occur a bad alloc or smth
        }
    }
    catch (...) {
        delete []new_data;
        throw;
    }
    
    delete[] data;
    size = other.size;
    capacity = other.capacity;
    data = new_data;

    return *this;
}

/*******************************************************************
 * getters
 *******************************************************************/

template <typename T>
const T& DynamicArray<T>::get(int index) const {
    if (index >= size || index < 0) {
        throw std::out_of_range("get: Index out of range");
    }
    
    return data[index];
}

// PERF git_size/get_capacity are the same for all 'T', but each 'T' will have it's own getter
template <typename T>
int DynamicArray<T>::get_size() const{
    return size;
}

template <typename T>
int DynamicArray<T>::get_capacity() const{
    return capacity;
}

/*******************************************************************
 * operations
 *******************************************************************/

template <typename T>
void DynamicArray<T>::set(int index, const T& value) {
    if (index >= size || index < 0) {
        throw std::out_of_range("set: Index out of range");
    }
    
    data[index] = value;  // do not use try-catch because 
}

template <typename T>
void DynamicArray<T>::remove_at(int index) {
    if (index < 0 || index >= size)
        throw std::out_of_range("remove_at: index out of range");

    // store the values, don't have to "delete"
    // no strong garanty, no mem leaks, but object can be changed incorrectly,
    // if "=" will fail
    for (int dst = index; dst + 1 < size; dst++) {
        data[dst] = data[dst + 1];
    }
    resize(size-1);
}

// changes the logic size of array (size).
// capacity increases (*2) automatically if don't have enough.
// does NOT decrease the capacity. Only if resize(0): totaly clear the buffer
template <typename T>
void DynamicArray<T>::resize(int new_size) {
    if (new_size < 0)
        throw std::invalid_argument("resize: new_size cannot be negative");

    // totaly clears the buffer (data)
    if (new_size == 0) {
        delete []data;
        data = nullptr;
        size = capacity = 0;
        return;
    }

     // capacity fits, just change the size    
    if (new_size <= capacity) {
        size = new_size;
        return;
    }

    // need more capacity for all new_size: allocate more
    int new_capacity = (new_size > capacity* 2) ? new_size : capacity * 2;
    T* new_data = new T[new_capacity]{};

    // fallback if the '=' operator for T will fall 
    try {
        for (int i = 0; i < size; i++) {
            new_data[i] = data[i];
        }
    } catch (...) {
        delete []new_data;
        throw;
    }
    
    delete []data;
    data = new_data;
    size = new_size;
    capacity = new_capacity;
}