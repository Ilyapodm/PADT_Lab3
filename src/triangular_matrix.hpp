#pragma once

#include "dynamic_array.hpp"
#include "square_matrix.hpp"
#include "math_types.hpp"

template <typename T>
class TriangularMatrix : public IMatrix<T> {
public:
    // kind, use enum class for incapsulation
    enum class Kind { Lower, Upper };

    // constructors
    TriangularMatrix();
    TriangularMatrix(int n, Kind kind);
    TriangularMatrix(T* items, int n, Kind kind);
    TriangularMatrix(const TriangularMatrix<T> &other);
    TriangularMatrix& operator=(const TriangularMatrix&) = default;
    ~TriangularMatrix() override {}  // don't need to delete data, because it's not pointer

    // getters
    const T& get(int i, int j) const override;  // can return static zero
    
    int get_rows() const override;  // returns the number of "строк"
    int get_cols() const override;  // returns the number of "столбцов"
    int get_size() const;

    // operations
    void set(int i, int j, const T &value) override;

    SquareMatrix<T>* add(const IMatrix<T> &other) const override;
    TriangularMatrix<T>* mult_scalar(const T &value) const override;  //TODO operator * и + 
    double norm() const override;
private:
    static int checked_size(int n);

    // n(n+1)/2 not zero elements
    // for Lower use Raw Major: Matrix(i, j) = data[i*(i+1)/2 + j]
    // for Upper use Column Major: Matrix(i, j) = data[j*(j+1)/2 + i]
    DynamicArray<T> data;  
    inline static const T ZERO = AlgebraTraits<T>::zero();  // one Zero for the class
    int n;  // rows and cols
    Kind kind;  // upper/lower
};

#include "triangular_matrix.tpp"