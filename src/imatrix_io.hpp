#pragma once

// separated file for streaming
#include <iostream>
#include "imatrix.hpp"

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
