#pragma once

// value type
template <typename T>
struct Complex{
    T re, im;
    Complex(T re = 0, T im = 0) : re{re}, im{im} {}

    // return by value for value tupes is normal
    Complex operator+(const Complex &o) const { return {re + o.re, im + o.im}; }
    Complex operator-(const Complex &o) const { return {re - o.re, im - o.im}; }
    Complex operator*(const Complex &o) const { return {re * o.re - im * o.im, re * o.im + im * o.re}; }
    Complex operator/(const Complex &o) const { 
        T div = o.re * o.re + o.im * o.im;
        return {(re * o.re + im * o.im) / div, (im * o.re - re * o.im) / div};
    }
    bool operator==(const Complex &o) const { return re == o.re && im == o.im; }
    //TODO реализовать оператор =
};