#pragma once

#include "vector.hpp"
#include "square_matrix.hpp"
#include <ostream>

template <typename T>
class SystemOfEquations {
public:
    // constructor
    SystemOfEquations(const SquareMatrix<T>& A, const Vector<T>& b);
    
    // getters
    const SquareMatrix<T>& get_matrix() const;
    const Vector<T>& get_rhs() const;  // returns vector b
    int get_size() const;

    // setters
    void set(const SquareMatrix<T> &new_A, const Vector<T> &new_b);  // allows to change A's and b's size; resets cache
    void set_matrix(const SquareMatrix<T> &new_A);  // new_A's size must match b; resets cache
    void set_rhs(const Vector<T> &new_b);  // new_b's size must match A; doesn't reset cache

    // methods to solute the SLAE
    Vector<T>* solve_gauss() const;
    Vector<T>* solve_gauss_with_pivot() const;  
    Vector<T>* solve_lu() const;     
    
    // fabrics
    static SystemOfEquations<T> random(int n, unsigned seed = 42);
    static SystemOfEquations<T> hilbert(int n);

private:
    SquareMatrix<T> A;
    Vector<T> b;    // vector col of free nums (rhs)

    void decompose_lu();  // being called only from solve_lu

    // LU-cache
    bool lu_ready = false;
    SquareMatrix<T> L;  // low triangle
    SquareMatrix<T> U;  // high triangle
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const SystemOfEquations<T>& sys) {
    SquareMatrix<T> m = sys.get_matrix();
    Vector<T> v = sys.get_rhs();
    for (int row = 0; row < sys.get_size(); row++) {
        os << "( ";
        for (int col = 0; col < sys.get_size() - 1; col++)  
            os << m.get(row, col) << ", ";
        if (sys.get_size() > 0)
            os << m.get(row, sys.get_size() - 1);
        os << " )  ( ";
        os << v.get(row) << " )";
    }
    return os;
}

#include "system_of_equations.tpp"