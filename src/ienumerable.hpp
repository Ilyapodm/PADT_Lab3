#pragma once
#include "ienumerator.hpp"

// interface of iterable collection
template <typename T>
class IEnumerable {
public:
    virtual ~IEnumerable() {};  // TODO отдельная папка для файлов из 2 лабы)
    virtual IEnumerator<T>* get_enumerator() const = 0;
};