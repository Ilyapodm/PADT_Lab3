#pragma once

#include "square_matrix.hpp"
#include "math_types.hpp"

/*******************************************************************
 * operations
 *******************************************************************/

template <typename T>
int SquareMatrix<T>::get_size() const {
    return this->get_cols();
}

template <typename T>
SquareMatrix<T>* SquareMatrix<T>::add(const IMatrix<T>& other) const {
    if (other.get_rows() != this->get_rows() || other.get_cols() != this->get_cols())
        throw std::invalid_argument("add: cols and rows must be the same");

    int n = this->get_rows();
    SquareMatrix<T>* result = new SquareMatrix<T>(n);
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
SquareMatrix<T>* SquareMatrix<T>::mult_scalar(const T& value) const {

    int n = this->get_rows();
    SquareMatrix<T>* result = new SquareMatrix<T>(n);

    try {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                result->set(i, j, this->get(i, j) * value);
            }
        }
    } catch(...) {
        delete result;
        throw;
    }
    
    return result;
}

template <typename T>
T SquareMatrix<T>::trace() const {
    T result = AlgebraTraits<T>::zero();
    for (int i = 0; i < this->get_rows(); i++)
        result = result + this->get(i, i);
    return result;
}

/*******************************************************************
 * elementary string transformations for SLAE
 *******************************************************************/

template <typename T>
void SquareMatrix<T>::swap_rows(int row1, int row2) {
    check_row(row1);
    check_row(row2);

    if (row1 == row2)
        return;

    T temp;
    for (int col = 0; col < this->get_cols(); col++) {
        temp = this->get(row1, col);
        this->set(row1, col, this->get(row2, col));
        this->set(row2, col, temp);
    }
}

template <typename T>
void SquareMatrix<T>::scale_row(int row, const T &factor) {
    check_row(row);

    for (int col = 0; col < this->get_cols(); col++) {
        this->set(row, col, this->get(row, col) * factor);
    }
}

template <typename T>
void SquareMatrix<T>::add_row(int src, int dst, const T &factor) {
    check_row(src);
    check_row(dst);

    // dst += factor * src
    for (int col = 0; col < this->get_cols(); col++) {
        this->set(dst, col, this->get(dst, col) + this->get(src, col) * factor);
    }
}  

/*******************************************************************
 * Utils
 *******************************************************************/

template <typename T>
void SquareMatrix<T>::check_row(int r) const {
        if (r < 0 || r >= this->get_rows())
            throw std::out_of_range("row index out of range");
    }