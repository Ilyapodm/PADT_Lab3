#pragma once

template <typename T>
class DynamicArray {
public:
    // constructors and operator=
    DynamicArray();
    DynamicArray(int size);
    DynamicArray(T* items, int size);
    DynamicArray(const DynamicArray<T> &other);
    ~DynamicArray();

    DynamicArray<T>& operator=(const DynamicArray<T> &other);

    // getters
    const T& get(int index) const;
    
    int get_size() const;
    int get_capacity() const;

    // operations
    void set(int index, const T& value);

    void remove_at(int index);

    // changes the logic size of array (size).
    // capacity increases (*2) automatically if don't have enough space for size 
    // (capacity == size -> capacity *= 2, size += 1)
    // does NOT decrease the capacity. Only if resize(0): totaly clear the buffer
    void resize(int new_size);  
    
private:
    T *data;
    int size;      // means the number of elements (reachable ceils of array in memory)
    int capacity;  // all allocated ceils of array in memory
};

#include "dynamic_array.tpp"