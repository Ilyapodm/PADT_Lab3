#pragma once

#include "ienumerator.hpp"

template <typename T>
class LinkedList {
private:  // here for enumerator access
    struct Node {
        T data;
        Node *next;
    };

public:
    // constructors 
    LinkedList();
    LinkedList(T *items, int size);
    LinkedList(const LinkedList<T> &other);
    ~LinkedList();

    LinkedList& operator=(const LinkedList& other);
    
    // getters 
    int get_length() const;

    const T& get_first() const;
    const T& get_last() const ;
    const T& get(int index) const;
    
    // operations
    void append(const T &item); 
    void prepend(const T &item);
    void insert_at(const T &item, int index);
    void set(int index, const T &item);
    
    T remove_at(int index);

    LinkedList<T>* get_sublist(int start_index, int end_index) const;

    LinkedList<T>* concat(const LinkedList<T> *other) const; 

    // heigher-order functions (map, where...)
    void filter(bool (*predicate)(const T&));  // "where" in list_sequence doesn't have the needed access (for not being O(n^2))
    void transform(T (*mapper)(const T&));  // map in list_sequence could be fast, but need set_current (mutable inumerator)

    // enumerator
    IEnumerator<T>* get_enumerator() const; // caller owns, must delete

    // nested class for enumerator
    class ListEnumerator : public IEnumerator<T> {
    public:
        ListEnumerator(const Node *head) : head{head}, current_node{nullptr}, started{0} {}

        bool move_next() override;  // move to next element
                
        const T& get_current() const override;  // get current item

        // void set_current(const T &value) override;  // set current item

        void reset() override;  // move to the beginning

    private:
        // we can change Node, but linked list will stay unchanged (length, tail, head will be the same)
        const Node *head;  // do not need the whole list
        bool started;  // not index
        const Node *current_node;  
    };

private:
    int length; 

    Node *head;
    Node *tail;
};

#include "linked_list.tpp"