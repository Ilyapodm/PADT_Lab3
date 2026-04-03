#pragma once

#include "dynamic_array.hpp"
#include "triangular_matrix.hpp"
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
    Vector<T>* solve_plu() const;     
    
    // fabrics
    static SystemOfEquations<T> random(int n, unsigned seed = 42);
    static SystemOfEquations<T> hilbert(int n);

    // metrics
    static double relative_error(const Vector<T> &approx, const Vector<T> &exact);  // static, because don't need A, b 
    double residual(const Vector<T> &x) const;  // not static, we need A, b to calculate this

private:
    SquareMatrix<T> A;
    Vector<T> b;    // vector col of free nums (rhs)

    void decompose_plu() const;  // being called only from solve_plu

    // PLU-cache (use mutable fields)
    // PA=LU -> PAx=Pb, LUx=PB
    mutable bool lu_ready = false;  
    mutable DynamicArray<int> P;  // permutations matrix (flat) for A
    mutable TriangularMatrix<T> L;  // low triangle
    mutable SquareMatrix<T> U;  // upper triangle
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