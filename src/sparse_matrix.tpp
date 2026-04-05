#pragma once

#include "math_types.hpp"
#include "sparse_matrix.hpp"
#include <stdexcept>

/*******************************************************************
 * constructors
 *******************************************************************/

template <typename T>
SparseMatrix<T>::SparseMatrix() : data{}, rows{0}, cols{0} {}

template <typename T>
SparseMatrix<T>::SparseMatrix(int rows, int cols) : 
    data{},
    rows{(check_dimensions(rows, cols), rows)},  // use comma operator
    cols{cols} 
{}

template <typename T>
SparseMatrix<T>::SparseMatrix(T* items, int rows, int cols, int count) :
    data{items, checked_size(rows, cols, count)},
    rows{rows},
    cols{cols}
{}

template <typename T>
SparseMatrix<T>::SparseMatrix(const SparseMatrix<T> &other) :
    data{other.data},
    rows{other.rows},
    cols{other.cols}
{}

/*******************************************************************
 * getters
 *******************************************************************/

template <typename T>
const T& SparseMatrix<T>::get(int i, int j) const {
    if (! in_bounds(i, j))
        throw std::out_of_range("get: index out of range");

    int index = bin_search(i, j);
    if (index == -1)
        return ZERO;
    return data.get(index).value;
}
    
template <typename T>
int SparseMatrix<T>::get_rows() const {
    return rows;
}

template <typename T>
int SparseMatrix<T>::get_cols() const {
    return cols;
}

template <typename T>
int SparseMatrix<T>::get_data_size() const {
    return data.get_size();
}

/*******************************************************************
 * operations
 *******************************************************************/
template <typename T>
void SparseMatrix<T>::set(int i, int j, const T &value) {
    if (! in_bounds(i, j))
        throw std::out_of_range("set: index out of range");

    int index = bin_search(i, j);

    // have to just rewrite the value
    if (index != -1) {
        if (value == AlgebraTraits<T>::zero()) {
            data.remove_at(index);
            return;
        }
        data.set(index, Triplet<T>{i, j, value});
    }

    // have to insert the value
    else {
        // don't have to add ZERO to data
        if (value == AlgebraTraits<T>::zero())
            return;

        data.resize(data.get_size() + 1);
        int dst = data.get_size() - 1;  // points to last element (index)

        // data is empty
        if (dst == 0)  {
            data.set(0, Triplet<T>{i, j, value});
            return;
        }
        
        while (dst > 0 &&
               ((data.get(dst - 1).i > i) || // left i is to big
               (data.get(dst - 1).i == i && data.get(dst - 1).j > j))  // i is good, j is too big
              ) 
        {
            data.set(dst, data.get(dst - 1));
            dst--;
        }

        data.set(dst, Triplet<T>{i, j, value});
    }
}

// PERF O(n^2) - никакой выгоды sparse здесь не дает из-за того, что мы ограничены интерфейсом
// а вообще можно было бы через dynamic cast проверять на SparseMatrix и мерджить массивы с триплетами
template <typename T>
SparseMatrix<T>* SparseMatrix<T>::add(const IMatrix<T> &other) const {
    if (rows != other.get_rows() || cols != other.get_cols())
        throw std::invalid_argument("add: cols and rows must be the same");

    SparseMatrix<T> *result = new SparseMatrix<T>(rows, cols);

    try {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result->set(i, j, get(i, j) + other.get(i, j));
            }
        }
    } catch(...) {
        delete result;
        throw;
    }
    
    return result;
}

template <typename T>
SparseMatrix<T>* SparseMatrix<T>::mult_scalar(const T &value) const {
    SparseMatrix<T> *result = new SparseMatrix<T>(rows, cols);

    if (value == AlgebraTraits<T>::zero())
        return result;

    result->data.resize(data.get_size());
    try {
        for (int i = 0; i < data.get_size(); i++) {
            Triplet<T> t = this->data.get(i);
            t.value = t.value * value;
            result->data.set(i, t);
        }
    } catch(...) {
        delete result;
        throw;
    }
    
    return result;
}

template <typename T>
double SparseMatrix<T>::norm() const {
    double res = 0.0;
    for (int i = 0; i < data.get_size(); i++) {
        res = res + (AlgebraTraits<T>::magnitude(data.get(i).value) *
                         AlgebraTraits<T>::magnitude(data.get(i).value));
    }
    return std::sqrt(res);
}

/*******************************************************************
 * helpers
 *******************************************************************/

// use not to create invalid object and then destroy it. 
// and also for rows * cols < 0 will be thrown dynamic_array 
// fallback: will cause misunderstanding.
template <typename T>
int SparseMatrix<T>::checked_size(int rows, int cols, int count) {
    check_dimensions(rows, cols); 
    if (count < 0)
        throw std::invalid_argument("SparseMatrix: count cannot be negative");
    return count;
}

template <typename T>
void SparseMatrix<T>::check_dimensions(int rows, int cols) {
    if (rows < 0 || cols < 0)
        throw std::invalid_argument("SparseMatrix: rows and cols cannot be negative");
}

template <typename T>
bool SparseMatrix<T>::in_bounds(int i, int j) const noexcept {
    return (i >= 0 && i < rows && j >= 0 && j < cols);
}

template <typename T>
int SparseMatrix<T>::bin_search(int i, int j) const {
    // i, j must be positive here (in_bounds helps)
    // returns the index of the element; -1 else
    if (data.get_size() == 0) return -1;

    int low = 0, high = data.get_size() - 1, mid;
    while (true) {
        mid = low + (high - low) / 2;
        // use i compare at first, than j compare
        if (data.get(mid).i > i || (data.get(mid).i == i && data.get(mid).j > j)) {
            high = mid - 1;
        }
        else if (data.get(mid).i < i || (data.get(mid).i == i && data.get(mid).j < j)) {
            low = mid + 1;
        }
        else 
            return mid;

        if (low > high)
            return -1;    
    }
}