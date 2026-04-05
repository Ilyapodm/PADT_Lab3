#pragma once

#include "imatrix.hpp"
#include "dynamic_array.hpp"
#include "math_types.hpp"

template <typename T>
struct Triplet {
    int i;
    int j;
    T value;
};

template <typename T>
class SparseMatrix : public IMatrix<T> {
public:
    // constructors
    SparseMatrix();
    SparseMatrix(int rows, int cols);
    SparseMatrix(T* items, int rows, int cols, int count);
    SparseMatrix(const SparseMatrix<T> &other);
    SparseMatrix& operator=(const SparseMatrix&) = default;
    ~SparseMatrix() override {}  // don't need to delete data, because it's not pointer

    // getters
    const T& get(int i, int j) const override;
    
    int get_rows() const override; 
    int get_cols() const override;  

    int get_data_size() const;  // returns the data's length (number of triplets)

    // operations
    void set(int i, int j, const T &value) override;

    SparseMatrix<T>* add(const IMatrix<T> &other) const override;
    SparseMatrix<T>* mult_scalar(const T &value) const override;
    double norm() const override;

private:
    inline static const T ZERO = AlgebraTraits<T>::zero();  // one Zero for the class
    DynamicArray<Triplet<T>> data;  // sorted (prior: 1. i, 2. j) array of triplets (i, j, data)
    int rows;
    int cols;

    static int checked_size(int rows, int cols, int count);
    bool in_bounds(int i, int j) const noexcept;
    int bin_search(int i, int j) const;  // returns the index of the element; -1 else
};

#include "sparse_matrix.tpp"