#pragma once

#include "sequence.hpp"
#include "ienumerator.hpp"
#include "linked_list.hpp"

template <typename T>
class ListSequence : public Sequence<T> {
public:
    // constructors
    ListSequence();
    ListSequence(T *items, int size);  //copy from given list
    ListSequence (const LinkedList<T> &list);
    ListSequence(const ListSequence<T> &other); 
    ~ListSequence() { delete list; }
    
    // Methods for Mutable/Immutable
    virtual ListSequence<T>* instance() = 0;  // returns who will be changed (copy or actually the object) 

    // need it because ArraySequence class is abstract (can't create objects), and in  
    // the methods we don't know mutable or immutable objects we are working with
    virtual ListSequence<T>* empty_sequence() const = 0;  // returns an empty Mutable or Immutable list sequence

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
    
    Sequence<T>* concat(const Sequence<T> &other) const override;

    // map, where, reduce
    Sequence<T>* map(T (*mapper)(const T &element)) override;
    Sequence<T>* where(bool (*predicate)(const T &element)) override;
    T reduce(T (*reduce_func)(const T &first_element, const T &second_element), const T &start_element) override;  
    Sequence<T>* slice(int i, int count, const Sequence<T> &seq) override;

    // enumerator    
    IEnumerator<T>* get_enumerator() const override { return this->list->get_enumerator(); } // caller owns, must delete

protected:
    LinkedList<T> *list;
}; 

template <typename T>
class MutableListSequence : public ListSequence<T> {
public:
    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(T *items, int size) : ListSequence<T>(items, size) {}
    MutableListSequence(const LinkedList<T> &list) : ListSequence<T>(list) {}
    MutableListSequence(const ListSequence<T> &other) : ListSequence<T>(other) {}
    MutableListSequence& operator=(const MutableListSequence &other) {
        if (this == &other) return *this;
        *this->list = *other.list; // LinkedList::operator=
        return *this;
    }
protected:
    ListSequence<T>* instance() override {
        return this;  // return actually the object
    }

    ListSequence<T>* empty_sequence() const override {
        return new MutableListSequence<T>();
    }
};

template <typename T>
class ImmutableListSequence : public ListSequence<T> {
public:
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(T *items, int size) : ListSequence<T>(items, size) {}
    ImmutableListSequence(const LinkedList<T> &list) : ListSequence<T>(list) {}
    ImmutableListSequence(const ListSequence<T> &other) : ListSequence<T>(other) {}
    ImmutableListSequence& operator=(const ImmutableListSequence &other) {
        if (this == &other) return *this;
        *this->list = *other.list; // LinkedList::operator=
        return *this;
    }
protected:
    ListSequence<T>* instance() override {
        return new ImmutableListSequence<T>(*this);  // return the copy
    }

    ListSequence<T>* empty_sequence() const override {
        return new ImmutableListSequence<T>();
    }
};

#include "list_sequence.tpp"