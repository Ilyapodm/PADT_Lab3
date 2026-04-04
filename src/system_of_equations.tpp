#pragma once

#include "dynamic_array.hpp"
#include "math_types.hpp"
#include "square_matrix.hpp"
#include "system_of_equations.hpp"
#include "triangular_matrix.hpp"
#include "vector.hpp"
#include <random>

/*******************************************************************
 * Constructors
 *******************************************************************/

template <typename T>
SystemOfEquations<T>::SystemOfEquations(const SquareMatrix<T>& A, const Vector<T>& b) : 
    A{A},
    b{b},
    lu_ready{false}
    // L{A.get_size(), TriangularMatrix<T>::Kind::Lower},
    // U{A}
{}

/*******************************************************************
 * Fabrics
 *******************************************************************/

template <typename T>
SystemOfEquations<T> SystemOfEquations<T>::random(int n, unsigned seed) {
    std::mt19937 rng(seed);  // random engine
    std::uniform_real_distribution<double> dist(-1, 1);

    SquareMatrix<T> A(n);
    Vector<T> b(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A.set(i, j, AlgebraTraits<T>::random_value(rng,  dist));
        }
        b.set(i, AlgebraTraits<T>::random_value(rng,  dist));
    }

    return SystemOfEquations<T>(A, b);
} 

template <typename T>
SystemOfEquations<T> SystemOfEquations<T>::hilbert(int n) {
    SquareMatrix<T> A(n);
    Vector<T> b(n);

    // x_exact = (1,1,...,1)^T, b = A * x_exact
    for (int i = 0; i < n; i++) {
        T row_sum = AlgebraTraits<T>::zero();
        for (int j = 0; j < n; j++) {
            // static_cast is legal, it can work with explicit constructor
            T val = static_cast<T>(1.0 / (i + j + 1));  // A[i][j] = 1/(i+j+1), 0-indexed
            A.set(i, j, val);
            row_sum = row_sum + val;  // b[i] = sum(A[i][j] * 1)
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
    if (new_A.get_rows() != new_b.get_size())
        throw std::invalid_argument("set: size mismatch between A and b");
    A = new_A;
    b = new_b;
    lu_ready = false;
}


template <typename T>
void SystemOfEquations<T>::set_matrix(const SquareMatrix<T>& new_A) {
    if (new_A.get_rows() != b.get_size())
        throw std::invalid_argument("set_matrix: size mismatch with b");
    A = new_A;
    lu_ready = false;  // change the flag
}

template <typename T>
void SystemOfEquations<T>::set_rhs(const Vector<T>& new_b) {
    if (new_b.get_size() != A.get_rows())
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
        if (pivot == AlgebraTraits<T>::zero())
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
Vector<T>* SystemOfEquations<T>::solve_gauss_with_pivot(double tol) const {
    SquareMatrix<T> temp_A = this->A;
    Vector<T> temp_b = this->b;
    const int n = this->get_size();

    // main col cycle
    for (int col = 0; col < n; ++col) {

        // search max row element |A[row][col]| for all row >= col
        int max_row = col;
        double max_val = AlgebraTraits<T>::magnitude(temp_A.get(col, col));
        for (int row = col + 1; row < n; row++) {
            double val = AlgebraTraits<T>::magnitude(temp_A.get(row, col));
            if (val > max_val) {
                max_val = val;
                max_row = row;
            }
        }

        // if too few - fall
        if (max_val < tol)
            throw std::runtime_error("solve_gauss_with_pivot: singular or near-singular matrix");

        // swap rows in temp_A and temp_b
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
        for (int col = row + 1; col < n; ++col)  // inserting already calculated variables
            sum -= temp_A.get(row, col) * result->get(col);
        result->set(row, sum / temp_A.get(row, row));
    }

    return result;
}

template <typename T>
Vector<T>* SystemOfEquations<T>::solve_plu(double tol) const {
    if (!lu_ready)
        decompose_plu(tol);

    const int n = get_size();
    // here b.get(i) = b.get(P.get(i)) to see origin row

    // forward: Ly = Pb 
    // L[i][i] == 1 do not divide
    Vector<T> y(n);
    for (int row = 0; row < n; row++) {
        T sum = b.get(P.get(row));
        for (int col = 0; col < row; col++)
            sum -= L.get(row, col) * y.get(col);
        y.set(row, sum);   // / L[i][i] == / 1 
    }

    // Backward Ux = y 
    Vector<T>* x = new Vector<T>(n);
    for (int row = n - 1; row >= 0; row--) {
        T sum = y.get(row);
        for (int col = row + 1; col < n; col++)
            sum -= U.get(row, col) * x->get(col);
        x->set(row, sum / U.get(row, row));
    }

    return x;
}

template <typename T>
void SystemOfEquations<T>::decompose_plu(double tol) const {
    const int n = get_size();

    P = DynamicArray<int>(n);  // P[i] = j: on the i row there is origin j row
    for (int i = 0; i < n; i++)  // set all indexes
        P.set(i, i);  
    L = TriangularMatrix<T>(n, TriangularMatrix<T>::Kind::Lower);

    // SquareMatrix is used in U because otherwise we have to create temp_A, 
    // which means O(n^2) + O(n*(n+1)/2) VS simply O(n^2)
    U = SquareMatrix<T>(A);

    // Doolittle's algorithm: L has 1 on the diagonal, U - upper matrix
    for (int col = 0; col < n; col++) {
        // col here is also like index of diagonal element

        // search max row element |A[row][col]| for all row >= col
        int max_row = col;
        double max_val = AlgebraTraits<T>::magnitude(U.get(col, col));

        for (int row = col + 1; row < n; row++) {
            double val = AlgebraTraits<T>::magnitude(U.get(row, col));
            if (val > max_val) {
                max_val = val;
                max_row = row;
            }
        }

        // if too few - fall
        if (max_val < tol)
            throw std::runtime_error("decompose_plu: singular or near-singular matrix");

        // swap rows in U, in L and indexes in P 
        if (max_row != col) {
            U.swap_rows(col, max_row);  // swap rows in U

            // swap indexes in P
            int temp_index = P.get(col);
            P.set(col, P.get(max_row));
            P.set(max_row, temp_index);

            // swap  L (cols: 0..col-1)
            for (int k = 0; k < col; ++k) {
                T tmp = L.get(col, k);
                L.set(col, k, L.get(max_row, k));
                L.set(max_row, k, tmp);
            }
        }

        //  forward
        L.set(col, col, AlgebraTraits<T>::one());
        const T pivot = U.get(col, col);
        for (int row = col + 1; row < n; ++row) {
            const T scale = U.get(row, col) / pivot;
            U.add_row(col, row, -scale);        // row += -scale * col
            L.set(row, col, scale);
        }
    }

    lu_ready = true;     
}

/*******************************************************************
 * Metrics
 *******************************************************************/

template <typename T>
double SystemOfEquations<T>::residual(const Vector<T> &x) const {
    const int n = get_size();

    // calculate r = Ax - b
    Vector<T> r(n);
    for (int i = 0; i < n; ++i) {
        T val = AlgebraTraits<T>::zero();
        for (int j = 0; j < n; ++j)
            val += A.get(i, j) * x.get(j);
        r.set(i, val - b.get(i));
    }

    // ||r|| / ||b|| — relative residual
    double norm_r = 0.0;
    double norm_b = 0.0;
    for (int i = 0; i < n; ++i) {
        norm_r += AlgebraTraits<T>::magnitude(r.get(i)) * AlgebraTraits<T>::magnitude(r.get(i));
        norm_b += AlgebraTraits<T>::magnitude(b.get(i)) * AlgebraTraits<T>::magnitude(b.get(i));
    }

    if (norm_b < 1e-14)
        throw std::runtime_error("residual: zero right-hand side, metric is undefined");

    return std::sqrt(norm_r) / std::sqrt(norm_b);
}

template <typename T>
double SystemOfEquations<T>::relative_error(const Vector<T> &approx, const Vector<T> &exact) {
    if (approx.get_size() != exact.get_size())
        throw std::invalid_argument("relative_error: vector size mismatch");

    const int n = approx.get_size();

    double norm_diff = 0.0;
    double norm_exact = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = AlgebraTraits<T>::magnitude(approx.get(i) - exact.get(i));
        double ex   = AlgebraTraits<T>::magnitude(exact.get(i));
        norm_diff += diff * diff;
        norm_exact += ex * ex;
    }

    if (norm_exact < 1e-14)
        throw std::runtime_error("relative_error: zero exact solution, metric is undefined");

    return std::sqrt(norm_diff) / std::sqrt(norm_exact);
}