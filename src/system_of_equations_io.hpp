#pragma once

#include "system_of_equations.hpp"
#include <iostream>

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
        os << v.get(row) << " )\n";
    }
    return os;
}