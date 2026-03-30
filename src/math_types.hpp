#pragma once

#include "complex.hpp"
#include <cmath>

template <typename T>
struct AlgebraTraits {
    static T zero() { return T{}; }
    static T one() { return T{1}; }
    static double magnitude(T x) { return std::abs(static_cast<double>(x)); }
};

template <typename T>
struct AlgebraTraits<Complex<T>> {
    static Complex<T> zero() { return Complex(T{}, T{}); }
    static Complex<T> one() { return Complex(T{1}, T{}); }
    static double magnitude(T x) { return x.abs(); }
};