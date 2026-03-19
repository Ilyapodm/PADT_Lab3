#pragma once

template <typename T>
class IMatrix {
public:
    virtual ~IMatrix() {}

    // getters
    virtual const T& get(int i, int j) const = 0;
    
    virtual int get_rows() const = 0;  // returns the number of "строк"
    virtual int get_cols() const = 0;  // returns the number of "столбцов"

    // operations
    virtual void set(int i, int j, const T &value) const = 0;

    virtual IMatrix* add(const IMatrix &other) const = 0;
    virtual IMatrix* mul_scalar(const T &value) const = 0;
    virtual T norm() const = 0;

    // operators
    const T& operator()(int i, int j) const { return get(i, j); }
    T& operator()(int i, int j)             { return get(i, j); }
};