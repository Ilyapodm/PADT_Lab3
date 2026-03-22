#pragma once

#include <stdexcept>
#include <cmath>

// value type
template <typename T>
struct Complex{
    T re, im;
    Complex(T re = T{}, T im = T{}) : re{re}, im{im} {}

    // return by value for value tupes is normal
    Complex operator+(const Complex &o) const { return {re + o.re, im + o.im}; }
    Complex operator-(const Complex &o) const { return {re - o.re, im - o.im}; }
    Complex operator-()                 const { return {-re, -im}; }  // unary minus
    Complex operator*(const Complex &o) const { return {re * o.re - im * o.im, re * o.im + im * o.re}; }
    Complex operator/(const Complex &o) const { 
        T div = o.re * o.re + o.im * o.im;
        if (div == T{0}) 
            throw std::domain_error("operator/: division by zero");
        return {(re * o.re + im * o.im) / div, (im * o.re - re * o.im) / div};
    }
    bool operator==(const Complex &o) const { return re == o.re && im == o.im; }
    bool operator!=(const Complex& o) const { return !(*this == o); }
    Complex& operator=(const Complex &o) {
        re = o.re;
        im = o.im;
        return *this;
    }
    T norm2() const { return re * re + im * im; }
    double abs() const { return std::sqrt(static_cast<double>(norm2())); }
    // use static_cast to return double 100%

};

template <typename T>
bool approx_equal(const Complex<T> &a, const Complex<T> &b, double eps = 1e-9) {
    return std::abs(a.re - b.re) < eps && std::abs(a.im - b.im) < eps;
}