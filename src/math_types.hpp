#pragma once

#include "complex.hpp"
#include <cmath>

// abs interface for int, double and other T
template <typename T>
double magnitude(T x) {
    return std::abs(static_cast<double>(x));
}

// abs interface for Complex<T>
template <typename T>
double magnitude(Complex<T> x) {
    return x.abs();
}

// file for collect math in one place