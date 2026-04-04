#pragma once

#include "imatrix.hpp"
#include "dynamic_array.hpp"
#include "square_matrix.hpp"

template <typename T>
class DiagonalMatrix : public IMatrix<T> {
public:
    // constructors
    DiagonalMatrix();
    explicit DiagonalMatrix(int n);
    DiagonalMatrix(T* items, int n);
    DiagonalMatrix(const DiagonalMatrix<T> &other);
    DiagonalMatrix& operator=(const DiagonalMatrix&) = default;
    ~DiagonalMatrix() override {}  // don't need to delete data, because it's not pointer

    // getters
    const T& get(int i, int j) const override;
    
    int get_rows() const override; 
    int get_cols() const override;  
    int get_size() const;

    // operations
    void set(int i, int j, const T &value) override;

    SquareMatrix<T>* add(const IMatrix<T> &other) const override;
    DiagonalMatrix<T>* mult_scalar(const T &value) const override;
    double norm() const override;
    
private:
    DynamicArray<T> data;  // store only diagonal elements
    int n;
    inline static const T ZERO = AlgebraTraits<T>::zero();

    static int checked_size(int n);
};

#include "diagonal_matrix.tpp"