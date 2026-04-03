#pragma once

#include "ienumerator.hpp"
#include "sequence.hpp"
#include <ostream>
#include <stdexcept>

// using const links for the object of exception
template <typename T>
Option<T> Sequence<T>::try_get_first() const {
    try {
        return Option<T>::Some(this->get_first());
    } catch (const std::out_of_range&) {
        return Option<T>::None();
    }
}

template <typename T>
Option<T> Sequence<T>::try_get_last() const {
    try {
        return Option<T>::Some(this->get_last());
    } catch (const std::out_of_range&) {
        return Option<T>::None();
    }
}

template <typename T>
Option<T> Sequence<T>::try_get(int index) const {
    try {
        return Option<T>::Some(this->get(index));
    } catch (const std::out_of_range&) {
        return Option<T>::None();
    }
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq) {
    os << "[";

    IEnumerator<T>* seq_iter = seq.get_enumerator();
    int i = 0;  // counter for putting commas
    while (seq_iter->move_next()) {
        os << seq_iter->get_current();
        if (i < seq.get_size() - 1)
            os << ", ";
        i++;
    }
    os << "]";

    delete seq_iter;

    return os;
}