#pragma once

#include <gtest/gtest.h>
#include <stdexcept>

#include "square_matrix.hpp"
#include "complex.hpp"

/*******************************************************************
 * Helpers
 *******************************************************************/

// fills 0,1,2,.. for numeric only (not complex)
template <typename T>
void fill_seq(SquareMatrix<T>& m) {
    int k = 0;
    for (int i = 0; i < m.get_rows(); i++)
        for (int j = 0; j < m.get_cols(); j++)
            m.set(i, j, static_cast<T>(k++));
}

// return filled matrix(n×n) 0,1,...
template <typename T>
SquareMatrix<T> make_seq(int n) {
    SquareMatrix<T> m(n);
    fill_seq(m);
    return m;
}

/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(SquareMatrixCtor, BasicSize) {
    SquareMatrix<double> m(4);
    EXPECT_EQ(m.get_rows(), 4);
    EXPECT_EQ(m.get_cols(), 4);
}

TEST(SquareMatrixCtor, ZeroSize) {
    EXPECT_NO_THROW(SquareMatrix<double>(0));
}

TEST(SquareMatrixCtor, NegativeSizeThrows) {
    EXPECT_THROW(SquareMatrix<double>(-1), std::invalid_argument);
}

TEST(SquareMatrixCtor, DefaultInitialized) {
    SquareMatrix<double> m(3);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            EXPECT_DOUBLE_EQ(m.get(i, j), 0.0);
}

TEST(SquareMatrixCtor, FromRawArray) {
    double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    SquareMatrix<double> m(data, 3);
    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 5.0);
    EXPECT_DOUBLE_EQ(m.get(2, 2), 9.0);
}

TEST(SquareMatrixCtor, CopyIsDeep) {
    SquareMatrix<double> a(2);
    fill_seq(a);
    SquareMatrix<double> b(a);
    b.set(0, 0, 999.0);
    EXPECT_DOUBLE_EQ(a.get(0, 0), 0.0);  // a didn't changed
}

/*******************************************************************
 * add — covariant return SquareMatrix<T>*
 *******************************************************************/

TEST(SquareMatrixAdd, ReturnsSquareMatrix) {
    SquareMatrix<double> a(2), b(2);
    IMatrix<double>* result = a.add(b);
    // dynamic_cast shows: is it really SquareMatrix inside or not
    EXPECT_NE(dynamic_cast<SquareMatrix<double>*>(result), nullptr);
    delete result;
}

TEST(SquareMatrixAdd, BasicValues) {
    SquareMatrix<double> a(2), b(2);
    a.set(0, 0, 1.0); a.set(0, 1, 2.0);
    a.set(1, 0, 3.0); a.set(1, 1, 4.0);
    b.set(0, 0, 9.0); b.set(0, 1, 8.0);
    b.set(1, 0, 7.0); b.set(1, 1, 6.0);

    SquareMatrix<double>* result = a.add(b);
    EXPECT_DOUBLE_EQ(result->get(0, 0), 10.0);
    EXPECT_DOUBLE_EQ(result->get(0, 1), 10.0);
    EXPECT_DOUBLE_EQ(result->get(1, 0), 10.0);
    EXPECT_DOUBLE_EQ(result->get(1, 1), 10.0);
    delete result;
}

TEST(SquareMatrixAdd, SizeMismatchThrows) {
    SquareMatrix<double> a(2), b(3);
    EXPECT_THROW(a.add(b), std::invalid_argument);
}

TEST(SquareMatrixAdd, OriginalUnchanged) {
    SquareMatrix<double> a(2), b(2);
    fill_seq(a);
    SquareMatrix<double>* result = a.add(b);
    EXPECT_DOUBLE_EQ(a.get(0, 0), 0.0);
    delete result;
}

TEST(SquareMatrixAdd, CanAcceptRectangularIMatrix) {
    Matrix<double> rect(2, 3);
    SquareMatrix<double> sq(2);
    EXPECT_THROW(sq.add(rect), std::invalid_argument);
}

/*******************************************************************
 * mult_scalar — covariant return
 *******************************************************************/

TEST(SquareMatrixMultScalar, ReturnsSquareMatrix) {
    SquareMatrix<double> m(2);
    IMatrix<double>* result = m.mult_scalar(2.0);
    // dynamic_cast shows: is it really SquareMatrix inside or not
    EXPECT_NE(dynamic_cast<SquareMatrix<double>*>(result), nullptr);
    delete result;
}

TEST(SquareMatrixMultScalar, BasicValues) {
    SquareMatrix<double> m = make_seq<double>(2); 
    SquareMatrix<double>* result = m.mult_scalar(3.0);
    EXPECT_DOUBLE_EQ(result->get(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(result->get(0, 1), 3.0);
    EXPECT_DOUBLE_EQ(result->get(1, 0), 6.0);
    EXPECT_DOUBLE_EQ(result->get(1, 1), 9.0);
    delete result;
}

TEST(SquareMatrixMultScalar, ByZero) {
    SquareMatrix<double> m = make_seq<double>(3);
    SquareMatrix<double>* result = m.mult_scalar(0.0);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            EXPECT_DOUBLE_EQ(result->get(i, j), 0.0);
    delete result;
}

/*******************************************************************
 * trace
 *******************************************************************/

TEST(SquareMatrixTrace, ZeroMatrix) {
    SquareMatrix<double> m(3);
    EXPECT_DOUBLE_EQ(m.trace(), 0.0);
}

TEST(SquareMatrixTrace, IdentityMatrix) {
    SquareMatrix<double> m(3);
    m.set(0, 0, 1.0); m.set(1, 1, 1.0); m.set(2, 2, 1.0);
    EXPECT_DOUBLE_EQ(m.trace(), 3.0);
}

TEST(SquareMatrixTrace, KnownValues) {
    double data[] = {1, 2, 3, 4};
    SquareMatrix<double> m(data, 2);
    EXPECT_DOUBLE_EQ(m.trace(), 5.0);
}

TEST(SquareMatrixTrace, Size1) {
    SquareMatrix<double> m(1);
    m.set(0, 0, 42.0);
    EXPECT_DOUBLE_EQ(m.trace(), 42.0);
}

TEST(SquareMatrixTrace, Complex) {
    SquareMatrix<Complex<double>> m(2);
    m.set(0, 0, {1.0, 2.0});
    m.set(1, 1, {3.0, 4.0});
    Complex<double> tr = m.trace();
    EXPECT_DOUBLE_EQ(tr.re, 4.0);
    EXPECT_DOUBLE_EQ(tr.im, 6.0);
}

/*******************************************************************
 * swap_rows
 *******************************************************************/

TEST(SquareMatrixSwapRows, Basic) {
    double data[] = {1, 2, 3, 4};
    SquareMatrix<double> m(data, 2);
    m.swap_rows(0, 1);
    EXPECT_DOUBLE_EQ(m.get(0, 0), 3.0);
    EXPECT_DOUBLE_EQ(m.get(0, 1), 4.0);
    EXPECT_DOUBLE_EQ(m.get(1, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 2.0);
}

TEST(SquareMatrixSwapRows, SameRowIsNoop) {
    SquareMatrix<double> m = make_seq<double>(3);
    double before = m.get(1, 0);
    m.swap_rows(1, 1);
    EXPECT_DOUBLE_EQ(m.get(1, 0), before);
}

TEST(SquareMatrixSwapRows, OutOfRangeThrows) {
    SquareMatrix<double> m(3);
    EXPECT_THROW(m.swap_rows(-1, 0),  std::out_of_range);
    EXPECT_THROW(m.swap_rows(0, 3),   std::out_of_range);
    EXPECT_THROW(m.swap_rows(3, 0),   std::out_of_range);
}

TEST(SquareMatrixSwapRows, DoubleSwapRestores) {
    SquareMatrix<double> m = make_seq<double>(3);
    SquareMatrix<double> original(m);
    m.swap_rows(0, 2);
    m.swap_rows(0, 2);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            EXPECT_DOUBLE_EQ(m.get(i, j), original.get(i, j));
}

/*******************************************************************
 * scale_row
 *******************************************************************/

TEST(SquareMatrixScaleRow, Basic) {
    double data[] = {1, 2, 3, 4};
    SquareMatrix<double> m(data, 2);
    m.scale_row(0, 3.0);
    EXPECT_DOUBLE_EQ(m.get(0, 0), 3.0);
    EXPECT_DOUBLE_EQ(m.get(0, 1), 6.0);
    EXPECT_DOUBLE_EQ(m.get(1, 0), 3.0); 
    EXPECT_DOUBLE_EQ(m.get(1, 1), 4.0); 
}

TEST(SquareMatrixScaleRow, ByOne) {
    SquareMatrix<double> m = make_seq<double>(3);
    SquareMatrix<double> before(m);
    m.scale_row(1, 1.0);
    for (int j = 0; j < 3; j++)
        EXPECT_DOUBLE_EQ(m.get(1, j), before.get(1, j));
}

TEST(SquareMatrixScaleRow, ByZero) {
    SquareMatrix<double> m = make_seq<double>(3);
    m.scale_row(2, 0.0);
    for (int j = 0; j < 3; j++)
        EXPECT_DOUBLE_EQ(m.get(2, j), 0.0);
}

TEST(SquareMatrixScaleRow, OutOfRangeThrows) {
    SquareMatrix<double> m(3);
    EXPECT_THROW(m.scale_row(-1, 2.0), std::out_of_range);
    EXPECT_THROW(m.scale_row(3,  2.0), std::out_of_range);
}

/*******************************************************************
 * add_row  (dst += factor * src)
 *******************************************************************/

TEST(SquareMatrixAddRow, Basic) {
    SquareMatrix<double> m(2);
    m.set(0, 0, 1.0); m.set(1, 1, 1.0);
    m.add_row(0, 1, 5.0);
    EXPECT_DOUBLE_EQ(m.get(1, 0), 5.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);  // src didn't change
}

TEST(SquareMatrixAddRow, SrcUnchanged) {
    double data[] = {1, 2, 3, 4};
    SquareMatrix<double> m(data, 2);
    m.add_row(0, 1, 2.0);
    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(0, 1), 2.0);
}

TEST(SquareMatrixAddRow, ZeroFactor) {
    SquareMatrix<double> m = make_seq<double>(3);
    SquareMatrix<double> before(m);
    m.add_row(0, 2, 0.0);
    for (int j = 0; j < 3; j++)
        EXPECT_DOUBLE_EQ(m.get(2, j), before.get(2, j));
}

TEST(SquareMatrixAddRow, GaussianEliminationStep) {
    double data[] = {2, 1, 6, 4};
    SquareMatrix<double> m(data, 2);
    m.add_row(0, 1, -3.0);
    EXPECT_NEAR(m.get(1, 0), 0.0, 1e-9);
    EXPECT_NEAR(m.get(1, 1), 1.0, 1e-9);
}

TEST(SquareMatrixAddRow, OutOfRangeThrows) {
    SquareMatrix<double> m(3);
    EXPECT_THROW(m.add_row(-1, 0, 1.0), std::out_of_range);
    EXPECT_THROW(m.add_row(0, -1, 1.0), std::out_of_range);
    EXPECT_THROW(m.add_row(3,  0, 1.0), std::out_of_range);
    EXPECT_THROW(m.add_row(0,  3, 1.0), std::out_of_range);
}
