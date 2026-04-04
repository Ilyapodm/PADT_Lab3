#pragma once

template <typename T>
class IMatrix {
    // Matrix is a Ring, so there are the requirements for T:
    // T{} must be additive zero
    // T{1} must be multiplicative zero
public:
    virtual ~IMatrix() {}

    // getters
    virtual const T& get(int i, int j) const = 0;
    
    virtual int get_rows() const = 0; 
    virtual int get_cols() const = 0;

    // operations
    virtual void set(int i, int j, const T &value) = 0;

    virtual IMatrix* add(const IMatrix &other) const = 0;    // returns a heap-allocated object, must be deleted by user
    virtual IMatrix* mult_scalar(const T &value) const = 0;  // returns a heap-allocated object, must be deleted by user
    virtual double norm() const = 0;  // Frobenius's norm

    // operators
    const T& operator()(int i, int j) const { return get(i, j); }  // reading only
};
