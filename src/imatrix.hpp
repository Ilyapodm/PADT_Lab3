#pragma once

#include <iostream>

template <typename T>
class IMatrix {
    // T{} must be additive zero
public:
    virtual ~IMatrix() {}

    // getters
    virtual const T& get(int i, int j) const = 0;
    
    virtual int get_rows() const = 0;  // returns the number of "строк"
    virtual int get_cols() const = 0;  // returns the number of "столбцов"

    // operations
    virtual void set(int i, int j, const T &value) = 0;

    virtual IMatrix* add(const IMatrix &other) const = 0;    // returns a heap-allocated object, must be deleted by user
    virtual IMatrix* mult_scalar(const T &value) const = 0;  // returns a heap-allocated object, must be deleted by user
    virtual double norm() const = 0;  // Frobenius's norm

    // operators
    const T& operator()(int i, int j) const { return get(i, j); }  // reading only
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const IMatrix<T>& m) {
    for (int row = 0; row < m.get_rows(); row++) {
        os << "( ";
        for (int col = 0; col < m.get_cols() - 1; col++)  
            os << m.get(row, col) << ", ";
        if (m.get_cols() > 0)
            os << m.get(row, m.get_cols() - 1);
        os << " )\n";
    }
    return os;
}
