#pragma once

#include "diagonal_matrix.hpp"

/*******************************************************************
 * constructors
 *******************************************************************/

template <typename T>
DiagonalMatrix<T>::DiagonalMatrix() : data{}, n{0} {}
    
template <typename T>
DiagonalMatrix<T>::DiagonalMatrix(int n) : 
    data{checked_size(n)},
    n{n}
{}

template <typename T>
DiagonalMatrix<T>::DiagonalMatrix(T *items, int n) :
    data{items, checked_size(n)},
    n{n}
{}

template <typename T>
DiagonalMatrix<T>::DiagonalMatrix(const DiagonalMatrix<T> &other) : 
    data{other.data},
    n{other.n}
{}

/*******************************************************************
 * getters
 *******************************************************************/
template <typename T>
const T& DiagonalMatrix<T>::get(int i, int j) const {
    if (i < 0 || j < 0 || i >= n || j >= n)
        throw std::out_of_range("get: index out of range");
    if (i != j)
        return ZERO;
    return data[i];
}

template <typename T>
int DiagonalMatrix<T>::get_rows() const {
    return n;
}

template <typename T>
int DiagonalMatrix<T>::get_cols() const {
    return n;
}

template <typename T>
int DiagonalMatrix<T>::get_size() const {
    return n;
}
/*******************************************************************
 * operations
 *******************************************************************/

template <typename T>
void DiagonalMatrix<T>::set(int i, int j, const T &value) {
    if (i < 0 || j < 0 || i >= n || j >= n)
        throw std::out_of_range("set: index out of range");

    if (i != j)
        throw std::invalid_argument("set: unable to set element to not diagonal position");

    data.set(i, value);
}

template <typename T>
SquareMatrix<T>* DiagonalMatrix<T>::add(const IMatrix<T> &other) const {
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
DiagonalMatrix<T>* DiagonalMatrix<T>::mult_scalar(const T &value) const {

    DiagonalMatrix<T> *result = new DiagonalMatrix<T>(n);

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
double DiagonalMatrix<T>::norm() const {
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
int DiagonalMatrix<T>::checked_size(int n) {
    if (n < 0)
        throw std::invalid_argument("DiagonalMatrix: n cannot be negative");
    // n can't overflow the int, because it is already int

    return n;
}