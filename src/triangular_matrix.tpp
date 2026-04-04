#pragma once

#include "triangular_matrix.hpp"
#include <climits>
#include <stdexcept>

/*******************************************************************
 * constructors
 *******************************************************************/

template <typename T>
TriangularMatrix<T>::TriangularMatrix() : data{}, n{0} {}

template <typename T>
TriangularMatrix<T>::TriangularMatrix(int n, Kind kind) :
    data{checked_size(n)},
    n{n},
    kind{kind}
{}

template <typename T>
TriangularMatrix<T>::TriangularMatrix(T *items, int n, Kind kind) :
    data{items, checked_size(n)},
    n{n},
    kind{kind}
{}

template <typename T>
TriangularMatrix<T>::TriangularMatrix(const TriangularMatrix<T> &other) :
    data{other.data},
    n{other.n},
    kind{other.kind}
{}

/*******************************************************************
 * getters
 *******************************************************************/

template <typename T>
const T& TriangularMatrix<T>::get(int i, int j) const {
    if (i < 0 || j < 0 || i >= n || j >= n)
        throw std::out_of_range("get: index out of range");
    bool is_lower = kind == Kind::Lower ? true : false;
    if ((i > j && !is_lower) || (j > i && is_lower))
        return ZERO;
    if (is_lower)
        return data[i * (i + 1) / 2 + j];
    return data[j * (j + 1) / 2 + i];
}

template <typename T>
int TriangularMatrix<T>::get_rows() const {
    return n;
}

template <typename T>
int TriangularMatrix<T>::get_cols() const {
    return n;
}

template <typename T>
int TriangularMatrix<T>::get_size() const {
    return n;
}

/*******************************************************************
 * operations
 *******************************************************************/
// for Lower use Raw Major: Matrix(i, j) = data[i*(i+1)/2 + j]
// for Upper use Column Major: Matrix(i, j) = data[j*(j+1)/2 + i]

// TODO убрать копипаст в оператор  ()
template <typename T>
void TriangularMatrix<T>::set(int i, int j, const T &value) {
    if (i < 0 || j < 0 || i >= n || j >= n)
        throw std::out_of_range("set: index out of range");

    bool is_lower = kind == Kind::Lower ? true : false;
    if ((is_lower && j > i) || (! is_lower && i > j))
        throw std::invalid_argument("set: unable to set element to not triangular position");

    if (is_lower) {
        data[i * (i + 1) / 2 + j] = value;
    } else {
        data[j * (j + 1) / 2 + i] = value;
    }
}

template <typename T>
SquareMatrix<T>* TriangularMatrix<T>::add(const IMatrix<T> &other) const {
    if (n != other.get_rows() || n != other.get_cols())
        throw std::invalid_argument("add: cols and rows must be the same");

    SquareMatrix<T> *result = new SquareMatrix<T>(n);

    try {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                result->set(i, j, this->get(i, j) + other.get(i, j));
    } catch(...) { 
        delete result;
        throw;
    }

    return result;
}

template <typename T>
TriangularMatrix<T>* TriangularMatrix<T>::mult_scalar(const T &value) const {
    TriangularMatrix<T>* result = new TriangularMatrix<T>(n, kind);
    bool is_lower = kind == Kind::Lower ? true : false;

    try {
        for (int i = 0; i < data.get_size(); i++) {
            result->data.set(i, data.get(i) * value);
        }
    } catch(...) {
        delete result;
        throw;
    }

    return result;
}

template <typename T>
double TriangularMatrix<T>::norm() const {
    double res = 0.0;
    for (int i = 0; i < data.get_size(); i++) {
            res = res + (AlgebraTraits<T>::magnitude(data.get(i)) *
                         AlgebraTraits<T>::magnitude(data.get(i)));
    }
    return std::sqrt(res);
}

/*******************************************************************
 * utils
 *******************************************************************/

// use not to create invalid object and then destroy it. 
template <typename T>
int TriangularMatrix<T>::checked_size(int n) {
    if (n < 0)
        throw std::invalid_argument("TriangularMatrix: n cannot be negative");
    if (n > 0 && n > INT_MAX / n)  // check this way not to do n*n, that overflows
        throw std::overflow_error("TriangularMatrix: n * n overflows int");

    return n * (n + 1) / 2;
}