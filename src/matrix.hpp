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
    ~Matrix() override {}  // don't need to delete data, because it's not pointer

    // getters
    const T& get(int i, int j) const override;
    
    int get_rows() const override;  // returns the number of "строк"
    int get_cols() const override;  // returns the number of "столбцов"

    // operations
    void set(int i, int j, const T &value) override;

    IMatrix<T>* add(const IMatrix<T> &other) const override;
    IMatrix<T>* mult_scalar(const T &value) const override;
    double norm() const override;
private:
    static int checked_size(int rows, int cols);
    int rows;
    int cols;

    // flat dynamic array for being cache friendly and not having extra iterators through rows
    DynamicArray<T> data;  // row major order, use data[i][j] = i * cols + j
};

#include "matrix.tpp"