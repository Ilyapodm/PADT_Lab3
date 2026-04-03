#pragma once

#include "matrix.hpp"
#include "math_types.hpp"
#include <stdexcept>
#include <cmath>
#include <climits>  // used for INT_MAX

/*******************************************************************
 * constructors
 *******************************************************************/

template <typename T>
Matrix<T>::Matrix() : data{}, rows{0}, cols{0} {}

template <typename T>
Matrix<T>::Matrix(int rows, int cols) : 
    data{checked_size(rows, cols)}, 
    rows{rows}, 
    cols{cols}
{}

template <typename T>
Matrix<T>::Matrix(T* items, int rows, int cols) : 
    data{items, checked_size(rows, cols)}, 
    rows{rows}, 
    cols{cols} 
{}

template <typename T>
Matrix<T>::Matrix(const Matrix<T> &other) : 
    data{other.data}, 
    rows{other.rows}, 
    cols{other.cols} 
{}

/*******************************************************************
 * getters
 *******************************************************************/

template <typename T>
const T& Matrix<T>::get(int i, int j) const {
    if (i < 0 || j < 0 || i >= rows || j >= cols)
        throw std::out_of_range("get: index out of range");

    return data[i * cols + j];
}

template <typename T>
int Matrix<T>::get_rows() const {
    return rows;
}

template <typename T>
int Matrix<T>::get_cols() const {
    return cols;
}

/*******************************************************************
 * operations
 *******************************************************************/

template <typename T>
void Matrix<T>::set(int i, int j, const T &value) {
    if (i < 0 || j < 0 || i >= rows || j >= cols)
        throw std::out_of_range("set: index out of range");
    
    data[i * cols + j] = value;  // operator = for T may throw
}

template <typename T>
Matrix<T>* Matrix<T>::add(const IMatrix<T> &other) const {
    if (rows != other.get_rows() || cols != other.get_cols())
        throw std::invalid_argument("add: cols and rows must be the same");

    Matrix<T> *result = new Matrix<T>(rows, cols);

    try {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result->set(i, j, this->get(i, j) + other.get(i, j));
            }
        }
    } catch(...) {
        delete result;
        throw;
    }
    
    return result;
}

template <typename T>
Matrix<T>* Matrix<T>::mult_scalar(const T &value) const {
    Matrix<T> *result = new Matrix<T>(rows, cols);

    try {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result->set(i, j, this->get(i, j) * value);
            }
        }
    } catch(...) {
        delete result;
        throw;
    }
    
    return result;
}

// Frobenius's Norm
template <typename T>
double Matrix<T>::norm() const {
    double res = 0.0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            res = res + (AlgebraTraits<T>::magnitude(this->get(i, j)) *
                         AlgebraTraits<T>::magnitude(this->get(i, j)));
        }
    }
    return std::sqrt(res);
}

/*******************************************************************
 * utils
 *******************************************************************/

// use not to create invalid object and then destroy it. 
// and also for rows * cols < 0 will be thrown dynamic_array 
// fallback: will cause misunderstanding.
template <typename T>
int Matrix<T>::checked_size(int rows, int cols) {
    if (rows < 0 || cols < 0)
        throw std::invalid_argument("Matrix: rows and cols cannot be negative");
    if (rows > 0 && cols > INT_MAX / rows)
        throw std::overflow_error("Matrix: rows * cols overflows int");

    return rows * cols;
}