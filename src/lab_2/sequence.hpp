#pragma once

#include "option.hpp"
#include "ienumerable.hpp"

template <typename T>
class Sequence : public IEnumerable<T> {
public:
    // getters
    virtual const T& get_first() const = 0;  // pure virtual method (have to be realized in child-classes)
    virtual const T& get_last() const = 0;
    virtual const T& get(int index) const = 0;

    virtual int get_size() const = 0;

    // try_... with Option<T> return
    Option<T> try_get_first() const;
    Option<T> try_get_last() const;
    Option<T> try_get(int index) const;

    // operations
    virtual Sequence<T>* append(const T &item) = 0;
    virtual Sequence<T>* prepend(const T &item) = 0;
    virtual Sequence<T>* insert_at(const T &item, int index) = 0;
    virtual Sequence<T>* set(const T &item, int index) = 0;
    
    virtual Sequence<T>* remove_at(int index) = 0;

    virtual Sequence<T>* get_subsequence(int start_index, int end_index) const = 0;

    virtual Sequence<T>* concat(const Sequence<T> &list) const = 0;

    // map, where, reduce
    virtual Sequence<T>* map(T (*mapper)(const T& element)) = 0;
    virtual Sequence<T>* where(bool (*predicate)(const T& element)) = 0;
    virtual T reduce(T (*reduce_func)(const T& first_element, const T& second_element), const T& start_element) = 0;
    virtual Sequence<T>* slice(int index, int count, const Sequence<T> &seq) = 0;

    // for reading only, because else "instance" approach is useless or need workarounds
    const T& operator[](int index) const { return this->get(index); } 

    virtual ~Sequence() {}
};

#include "sequence.tpp" 