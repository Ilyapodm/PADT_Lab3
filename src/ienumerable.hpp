#pragma once
#include "ienumerator.hpp"

// interface of iterable collection
template <typename T>
class IEnumerable {
public:
    virtual ~IEnumerable() {};
    virtual IEnumerator<T>* get_enumerator() const = 0;
};