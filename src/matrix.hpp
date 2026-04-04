#pragma once

#include "imatrix.hpp"
#include "dynamic_array.hpp"

template <typename T>
class Matrix : public IMatrix<T> {
public:
    // constructors
    Matrix();
    Matrix(int rows, int cols);
    Matrix(T* items, int rows, int cols);
    Matrix(const Matrix<T> &other);
    Matrix& operator=(const Matrix&) = default;
    ~Matrix() override {}  // don't need to delete data, because it's not pointer

    // getters
    const T& get(int i, int j) const override;
    
    int get_rows() const override;  // returns the number of "строк"
    int get_cols() const override;  // returns the number of "столбцов"

    // operations
    void set(int i, int j, const T &value) override;

    Matrix<T>* add(const IMatrix<T> &other) const override;
    Matrix<T>* mult_scalar(const T &value) const override;  
    double norm() const override;
    
private:
    // flat dynamic array for being cache friendly and not having extra iterators through rows
    DynamicArray<T> data;  // row major order, use data[i][j] = i * cols + j
    int rows;
    int cols;

    static int checked_size(int rows, int cols);
    bool in_bounds(int i, int j) const noexcept;
};

#include "matrix.tpp"