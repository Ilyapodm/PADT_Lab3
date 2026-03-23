#pragma once

#include "matrix.hpp"

template <typename T>
class SquareMatrix : public Matrix<T> {
public:
    // constructors
    explicit SquareMatrix(int n) : Matrix<T>(n, n) {}  // explicit to have no suprises with solo integers
    SquareMatrix(T* items, int n) : Matrix<T>(items, n, n) {}
    SquareMatrix(const SquareMatrix<T> &other) : Matrix<T>(other) {}
    ~SquareMatrix() override {}

    // setter and getters are the same in the Matrix
    // need to override add and mult_scalar because of return value

    SquareMatrix<T>* add(const IMatrix<T>& other) const override;
    SquareMatrix<T>* mult_scalar(const T& value) const override;

    // diagonal sum
    T trace() const;

    // elementary string transformations for SLAE
    void swap_rows(int row1, int row2);
    void scale_row(int row, const T& factor);
    void add_row(int src, int dst, const T &factor);  // dst += factor * src
private:
    void check_row(int row) const;
};

#include "square_matrix.tpp"