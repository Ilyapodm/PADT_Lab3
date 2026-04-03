#pragma once

#include "complex.hpp"
#include <cmath>
#include <random>

template <typename T>
struct AlgebraTraits {
    static T zero() { return T{}; }
    static T one() { return T{1}; }
    static double magnitude(T x) { return std::abs(static_cast<double>(x)); }
    static T random_value(std::mt19937 &rng, std::uniform_real_distribution<double> &dist) {
        return static_cast<T>(dist(rng));
    }
};

// partial specialization
template <typename T>
struct AlgebraTraits<Complex<T>> {
    static Complex<T> zero() { return Complex(AlgebraTraits<T>::zero(), AlgebraTraits<T>::zero()); }
    static Complex<T> one() { return Complex(AlgebraTraits<T>::one(), AlgebraTraits<T>::zero()); }
    static double magnitude(Complex<T> x) { return x.abs(); }
    static Complex<T> random_value(std::mt19937 &rng, std::uniform_real_distribution<double> &dist) {
        return Complex<T>(static_cast<T>(dist(rng)), static_cast<T>(dist(rng)));
    }
};