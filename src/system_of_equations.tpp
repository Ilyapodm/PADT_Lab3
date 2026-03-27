#pragma once

#include "square_matrix.hpp"
#include "system_of_equations.hpp"
#include "vector.hpp"
#include <random>

/*******************************************************************
 * Constructors
 *******************************************************************/

template <typename T>
SystemOfEquations<T>::SystemOfEquations(const SquareMatrix<T>& A, const Vector<T>& b) {}

/*******************************************************************
 * Fabrics
 *******************************************************************/

template <typename T>
SystemOfEquations<T> random(int n, unsigned seed = 42) {
    std::mt19937 rng(seed);  // random engine
    std::uniform_real_distribution<double> dist(-1, 1);

    SquareMatrix<T> A(n);
    Vector<T> b(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A.set(i, j, static_cast<T>(dist(rng)));
        }
        b.set(i, static_cast<T>(dist(rng)));
    }

    return SystemOfEquations<T>(A, b);
} 

template <typename T>
SystemOfEquations<T> hilbert(int n) {
    SquareMatrix<T> A(n);
    Vector<T> b(n);

    // x_exact = (1,1,...,1)^T, b = H * x_exact
    for (int i = 0; i < n; i++) {
        T row_sum = T{};
        for (int j = 0; j < n; j++) {
            T val = static_cast<T>(1.0 / (i + j + 1));  // H[i][j] = 1/(i+j+1), 0-indexed
            A.set(i, j, val);
            row_sum = row_sum + val;  // b[i] = sum(H[i][j] * 1)
        }
        b.set(i, row_sum);
    }

    return SystemOfEquations<T>(A, b);
}

/*******************************************************************
 * Getters
 *******************************************************************/

template <typename T>
const SquareMatrix<T>& SystemOfEquations<T>::get_matrix() const {
    return A;
}

template <typename T>
const Vector<T>& SystemOfEquations<T>::get_rhs() const {
    return b;
}

template <typename T>
int SystemOfEquations<T>::get_size() const {
    return A.get_size();
}

/*******************************************************************
 * setters
 *******************************************************************/

template <typename T>
void SystemOfEquations<T>::set(const SquareMatrix<T>& new_A, const Vector<T>& new_b) {
    if (new_A.get_rows() != new_b.size())
        throw std::invalid_argument("set: size mismatch between A and b");
    A = new_A;
    b = new_b;
    lu_ready = false;
}


template <typename T>
void SystemOfEquations<T>::set_matrix(const SquareMatrix<T>& new_A) {
    if (new_A.get_rows() != b.size())
        throw std::invalid_argument("set_matrix: size mismatch with b");
    A = new_A;
    lu_ready = false;  // change the flag
}

template <typename T>
void SystemOfEquations<T>::set_rhs(const Vector<T>& new_b) {
    if (new_b.size() != A.get_rows())
        throw std::invalid_argument("set_rhs: size mismatch with A");
    b = new_b;
    // don't reset lu_ready: LU depends on A, not on b
}

/*******************************************************************
 * methods to solute the SLAE
 *******************************************************************/

template <typename T>
Vector<T>* SystemOfEquations<T>::solve_gauss() const {
    SquareMatrix<T> temp_A = this->A;
    Vector<T> temp_b = this->b;
    const int n = this->get_size();

    // forward
    for (int col = 0; col < n; col++) {
        T pivot = temp_A.get(col, col);
        if (pivot == T{})
            throw std::runtime_error("solve_gauss: zero pivot, matrix may be singular");
        for (int row = col + 1; row < n; row++) {
            T scale = temp_A.get(row, col) / pivot;
            temp_A.add_row(col , row, -scale);
            temp_b.set(row, temp_b.get(row) - scale * temp_b.get(col));
        }
    }

    Vector<T> *result = new Vector<T>(n);

    // backward
    for (int row = n - 1; row >= 0; row--) {
        T sum = temp_b.get(row);
        for (int col = row + 1; col < n; col++) {
            sum = sum - temp_A.get(row, col) * result->get(col);
        }
        result->set(row, sum / temp_A.get(row, row));
    }

    return result;
}

template <typename T>
Vector<T>* SystemOfEquations<T>::solve_gauss_with_pivot() const {
    SquareMatrix<T> temp_A = this->A;
    Vector<T> temp_b = this->b;
    const int n = this->get_size();

    // choose max in the col
    for (int col = 0; col < n; ++col) {

        // search max row element |A[row][col]| for all r >= col
        int max_row = col;
        double max_val = std::abs(static_cast<double>(magnitude(temp_A.get(col, col))));
        for (int row = col + 1; row < n; row++) {
            double val = std::abs(static_cast<double>(magnitude(temp_A.get(row, col))));
            if (val > max_val) {
                max_val = val;
                max_row = row;
            }
        }

        // if too few - fall
        if (max_val < 1e-14)
            throw std::runtime_error("solve_gauss_with_pivot: singular or near-singular matrix");

        // 3. swap rows in temp_A and temp_b
        if (max_row != col) {
            temp_A.swap_rows(col, max_row);
            T tmp = temp_b.get(col);
            temp_b.set(col, temp_b.get(max_row));
            temp_b.set(max_row, tmp);
        }

        // actually forward
        const T pivot = temp_A.get(col, col);
        for (int row = col + 1; row < n; ++row) {
            const T scale = temp_A.get(row, col) / pivot;
            temp_A.add_row(col, row, -scale);        // row += -scale * col
            temp_b.set(row, temp_b.get(row) - scale * temp_b.get(col));
        }
    }

    Vector<T>* result = new Vector<T>(n);

    // backward
    for (int row = n - 1; row >= 0; --row) {
        T sum = temp_b.get(row);
        for (int col = row + 1; col < n; ++col)
            sum -= temp_A.get(row, col) * result->get(col);
        result->set(row, sum / temp_A.get(row, row));
    }

    return result;
}

template <typename T>
const Vector<T>* SystemOfEquations<T>::solve_lu() {

}

template <typename T>
void SystemOfEquations<T>::decompose_lu() {

}


