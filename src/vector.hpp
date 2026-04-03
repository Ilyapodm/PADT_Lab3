#pragma once

#include "matrix.hpp"

template <typename T>
class Vector : public Matrix<T> {
public:
    explicit Vector(int n) : Matrix<T>(n, 1) {}
    Vector(T* items, int n) : Matrix<T>(items, n, 1) {}
    Vector(const Vector<T>& other) : Matrix<T>(other) {}

    const T& operator[](int i) const {
        return Matrix<T>::get(i, 0);
    }

    // not an override, this is overload
    const T& get(int i) const {
        return Matrix<T>::get(i, 0); 
    }
    
    void set(int i, const T& value) { 
        Matrix<T>::set(i, 0, value); 
    }

    int get_size() const { 
        return Matrix<T>::get_rows(); 
    }

    // Covariant return
    Vector<T>* add(const IMatrix<T>& other) const override {
        if (other.get_rows() != this->get_rows() || other.get_cols() != this->get_cols())
            throw std::invalid_argument("add: cols and rows must be the same");

        int n = this->get_size();

        Vector<T>* result = new Vector<T>(n);
        try {
            for (int i = 0; i < n; i++)
                result->set(i, this->get(i) + other.get(i, 0));
        } catch(...) { 
            delete result;
            throw;
        }

        return result;
    }

    Vector<T>* mult_scalar(const T& value) const override {
        int n = this->get_size();
        Vector<T>* result = new Vector<T>(n);

        try {
            for (int i = 0; i < n; i++) {
                result->set(i, this->get(i) * value);
            }
        } catch(...) {
            delete result;
            throw;
        }
        
        return result;
    }
};
