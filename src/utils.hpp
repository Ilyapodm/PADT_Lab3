#pragma once

#include <iostream>
#include "array_sequence.hpp"
#include "sequence.hpp"

template <typename T1, typename T2>
struct Pair {
    T1 first;
    T2 second;

    Pair() = default;
    Pair(const T1 &first, const T2 &second) : first{first}, second{second} {}
    Pair(const Pair&) = default;
    Pair& operator=(const Pair&) = default;

    bool operator==(const Pair &other) {
        return (first == other.first) && (second == other.second);
    }
};

template <typename T1, typename T2> 
std::ostream& operator<<(std::ostream& os, const Pair<T1, T2> &pair) {
    os << "(" << pair.first << ", " << pair.second << ")";
    return os;
}

// User have to delete
// if sizes of sequences aren't equal, the result's size will be the minimum of the sequences
template <typename T1, typename T2>
MutableArraySequence<Pair<T1, T2>>* zip(const Sequence<T1> &first, const Sequence<T2> &second) {
    // choose min size
    int result_size = (first.get_size() > second.get_size()) ? second.get_size() : first.get_size();  

    MutableArraySequence<Pair<T1, T2>> *result = new MutableArraySequence<Pair<T1, T2>>();

    try {
        for (int i = 0; i < result_size; i++) {
            result->append({first[i], second[i]});
        }
    } catch (...) {
        delete result;
        throw;
    }
    
    return result;
}

// unzip returns a "Pair" of MutableArraySequences. Doens't matter what they were before
template <typename T1, typename T2>
Pair<MutableArraySequence<T1>*, MutableArraySequence<T2>* >
unzip(const Sequence<Pair<T1, T2>> &seq) {
    MutableArraySequence<T1> *res1 = new MutableArraySequence<T1>();
    MutableArraySequence<T2> *res2 = new MutableArraySequence<T2>();

    try {
        for (int i = 0; i < seq.get_size(); i++) {
        res1->append(seq[i].first);
        res2->append(seq[i].second);
        }
    } catch (...) {
        delete res1;
        delete res2;
        throw;
    }

    return {res1, res2};
}

template <typename T>
MutableArraySequence< MutableArraySequence<T>* >* split(const Sequence<T> &seq, const T &split_element) {
    MutableArraySequence< MutableArraySequence<T>* >* result = new MutableArraySequence< MutableArraySequence<T>* >();
    MutableArraySequence<T> *current = new MutableArraySequence<T>();

    try {
        for (int i = 0; i < seq.get_size(); i++) {
            if (seq[i] == split_element) {
                result->append(current);
                current = new MutableArraySequence<T>();
            }
            else {
                current->append(seq[i]);
            }
        }

        result->append(current);
        return result;
    } catch (...) {
        delete current;
        for (int i = 0; i < result->get_size(); i++) {
            delete (*result)[i];
        }
        delete result;
        throw;
    }
    
}