#pragma once

#include <cmath>

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

// abs interface for int, double and other T
template <typename T>
double magnitude(T x) {
    return std::abs(static_cast<double>(x));
}