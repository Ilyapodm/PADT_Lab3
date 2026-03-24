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
    const Vector<T>& get_rhs() const;  // returns b
    int size() const;

    // methods to solute the SLAE
    const Vector<T> &solve_gauss();
    const Vector<T> &solve_gauss_with_pivot();  
    const Vector<T> &solve_lu();                 

private:
    SquareMatrix<T> A;
    Vector<T> b;    // vector col of free nums (rhs)

    // LU-cache
    bool lu_ready = false;
    SquareMatrix<T> L;  // low triangle
    SquareMatrix<T> U;  // high triangle
};

// std::ostream& operator<<(std::ostream &os, const SystemOfEquations &sys) {
//     for (int i = 0; i < sys.get_number_variables(); i++) {

//     }
// }