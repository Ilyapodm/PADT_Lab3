#pragma once

#include "sequence.hpp"
#include "dynamic_array.hpp"

template <typename T>
class ArraySequence : public Sequence<T> {
public:
    // constructors
    ArraySequence();
    ArraySequence(T *items, int size);  // copy from given array
    ArraySequence(const DynamicArray<T> &array);
    ArraySequence(const ArraySequence<T> &other);
    ~ArraySequence() { delete array; }

    // do not write Sequence<T>* operator=(const ArraySequence<T> &other); because for Mutable 
    // and immutable there will be different args - "перегрузка", not redefinition.
    
    // methods for Mutable/Immutable
    virtual ArraySequence<T>* instance() = 0;  // returns who will be changed (copy or actually the object)

    // need it because ArraySequence class is abstract (can't create objects), and in  
    // the methods we don't know mutable or immutable objects we are working with
    virtual ArraySequence<T>* empty_sequence() const = 0;  // returns an empty Mutable or Immutable (array sequence)

    // getters
    const T& get_first() const override;  
    const T& get_last() const override;
    const T& get(int index) const override;

    int get_size() const override;
    
    // operations
    Sequence<T>* append(const T &item) override;
    Sequence<T>* prepend(const T &item) override;
    Sequence<T>* insert_at(const T &item, int index) override;
    Sequence<T>* set(const T &item, int index) override;

    Sequence<T>* remove_at(int index) override;

    Sequence<T>* get_subsequence(int start_index, int end_index) const override;

    Sequence <T>* concat(const Sequence<T> &other) const override;

    // map, where, reduce
    Sequence<T>* map(T (*mapper)(const T &element)) override;
    Sequence<T>* where(bool (*predicate)(const T &element)) override;
    T reduce(T (*reduce_func)(const T &first_element, const T &second_element), const T &start_element) override;
    Sequence<T>* slice(int i, int count, const Sequence<T> &seq) override;

    // nested class for enumerator
    class ArrayEnumerator : public IEnumerator<T> {
    public:
        ArrayEnumerator(const ArraySequence<T> *array_sequence) : array_sequence{array_sequence}, index{-1} {}

        bool move_next() override;  // move to next element
                
        const T& get_current() const override;  // get current item

        // void set_current(const T &value) override;  // set current item

        void reset() override;  // move to the beginning

    private:
        int index;
        const ArraySequence<T> *array_sequence;
    };

    // caller owns, must delete
    IEnumerator<T>* get_enumerator() const override;

protected:
    // actually all size and capacity info delegates to dynamic array
    DynamicArray<T> *array;
};

template <typename T>
class MutableArraySequence : public ArraySequence<T> {
public:
    MutableArraySequence() : ArraySequence<T>() {}
    MutableArraySequence(T *items, int size) : ArraySequence<T>(items, size) {}
    MutableArraySequence(const DynamicArray<T> &array) : ArraySequence<T>(array) {}
    MutableArraySequence(const ArraySequence<T> &other) : ArraySequence<T>(other) {}
    MutableArraySequence& operator=(const MutableArraySequence &other) {
        if (this == &other) return *this;
        *this->array = *other.array; // DynamicArray::operator=
        return *this;
    }

protected:
    ArraySequence<T>* instance() override {
        return this;  // return actually the object
    }

    ArraySequence<T>* empty_sequence() const override {
        return new MutableArraySequence<T>();
    }
};

template <typename T>
class ImmutableArraySequence : public ArraySequence<T> {
public:
    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(T *items, int size) : ArraySequence<T>(items, size) {}
    ImmutableArraySequence(const DynamicArray<T> &array) : ArraySequence<T>(array) {}
    ImmutableArraySequence(const ArraySequence<T> &other) : ArraySequence<T>(other) {}
    ImmutableArraySequence& operator=(const ImmutableArraySequence &other) {
        if (this == &other) return *this;
        *this->array = *other.array; // DynamicArray::operator=
        return *this;
    }
protected:
    ArraySequence<T>* instance() override {
        return new ImmutableArraySequence<T>(*this);  // return the copy
    }

    ArraySequence<T>* empty_sequence() const override {
        return new ImmutableArraySequence<T>();
    }
};

#include "array_sequence.tpp"