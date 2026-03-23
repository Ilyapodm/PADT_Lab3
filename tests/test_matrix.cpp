// test_matrix.cpp
#pragma once

#include <gtest/gtest.h>
#include <stdexcept>
#include <cmath>

#include "matrix.hpp"
#include "complex.hpp"

/*******************************************************************
 * Fixture
 *******************************************************************/

// fill matrix 0, 1, 2, 3...
template <typename T>
void fill_seq(Matrix<T>& m) {
    int k = 0;
    for (int i = 0; i < m.get_rows(); i++)
        for (int j = 0; j < m.get_cols(); j++)
            m.set(i, j, static_cast<T>(k++));
}

/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(MatrixConstructor, DefaultIsEmpty) {
    Matrix<double> m;
    EXPECT_EQ(m.get_rows(), 0);
    EXPECT_EQ(m.get_cols(), 0);
}

TEST(MatrixConstructor, ZeroSizeIsValid) {
    EXPECT_NO_THROW(Matrix<double>(0, 0));
}

TEST(MatrixConstructor, NegativeRowsThrows) {
    EXPECT_THROW(Matrix<double>(-1, 3), std::invalid_argument);
}

TEST(MatrixConstructor, NegativeColsThrows) {
    EXPECT_THROW(Matrix<double>(3, -1), std::invalid_argument);
}

TEST(MatrixConstructor, OverflowThrows) {
    // rows * cols > INT_MAX
    EXPECT_THROW(Matrix<int>(100000, 100000), std::overflow_error);
}

TEST(MatrixConstructor, SizeIsCorrect) {
    Matrix<double> m(3, 4);
    EXPECT_EQ(m.get_rows(), 3);
    EXPECT_EQ(m.get_cols(), 4);
}

TEST(MatrixConstructor, DefaultInitialized) {
    // T{} for double = 0.0
    Matrix<double> m(2, 2);
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            EXPECT_DOUBLE_EQ(m.get(i, j), 0.0);
}

TEST(MatrixConstructor, CopyIsDeep) {
    Matrix<double> a(2, 2);
    fill_seq(a);
    Matrix<double> b(a);
    b.set(0, 0, 999.0);
    // a is the same, copy 
    EXPECT_DOUBLE_EQ(a.get(0, 0), 0.0);
}

TEST(MatrixConstructor, FromRawArray) {
    double data[] = {1, 2, 3, 4, 5, 6};
    Matrix<double> m(data, 2, 3);
    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(1, 2), 6.0);
}

/*******************************************************************
 * get / set
 *******************************************************************/

TEST(MatrixGetSet, SetAndGet) {
    Matrix<double> m(3, 3);
    m.set(1, 2, 42.0);
    EXPECT_DOUBLE_EQ(m.get(1, 2), 42.0);
}

TEST(MatrixGetSet, GetOutOfRangeThrows) {
    Matrix<double> m(3, 3);
    EXPECT_THROW(m.get(-1, 0),  std::out_of_range);
    EXPECT_THROW(m.get(0, -1),  std::out_of_range);
    EXPECT_THROW(m.get(3, 0),   std::out_of_range);
    EXPECT_THROW(m.get(0, 3),   std::out_of_range);
}

TEST(MatrixGetSet, SetOutOfRangeThrows) {
    Matrix<double> m(3, 3);
    EXPECT_THROW(m.set(-1, 0, 1.0), std::out_of_range);
    EXPECT_THROW(m.set(0, 3,  1.0), std::out_of_range);
}

TEST(MatrixGetSet, OperatorCallEqualsGet) {
    Matrix<double> m(2, 2);
    m.set(0, 1, 7.0);
    EXPECT_DOUBLE_EQ(m(0, 1), m.get(0, 1));
}

/*******************************************************************
 * add
 *******************************************************************/

TEST(MatrixAdd, BasicAdd) {
    Matrix<double> a(2, 2), b(2, 2);
    a.set(0, 0, 1.0); a.set(0, 1, 2.0);
    a.set(1, 0, 3.0); a.set(1, 1, 4.0);
    b.set(0, 0, 5.0); b.set(0, 1, 6.0);
    b.set(1, 0, 7.0); b.set(1, 1, 8.0);

    IMatrix<double>* c = a.add(b);
    EXPECT_DOUBLE_EQ(c->get(0, 0), 6.0);
    EXPECT_DOUBLE_EQ(c->get(0, 1), 8.0);
    EXPECT_DOUBLE_EQ(c->get(1, 0), 10.0);
    EXPECT_DOUBLE_EQ(c->get(1, 1), 12.0);
    delete c;
}

TEST(MatrixAdd, SizeMismatchThrows) {
    Matrix<double> a(2, 3), b(3, 2);
    EXPECT_THROW(a.add(b), std::invalid_argument);
}

TEST(MatrixAdd, AddZeroMatrix) {
    Matrix<double> a(2, 2), zero(2, 2);
    fill_seq(a);
    IMatrix<double>* result = a.add(zero);
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            EXPECT_DOUBLE_EQ(result->get(i, j), a.get(i, j));
    delete result;
}

TEST(MatrixAdd, OriginalUnchanged) {
    Matrix<double> a(2, 2), b(2, 2);
    fill_seq(a);
    fill_seq(b);
    IMatrix<double>* c = a.add(b);
    // a не изменился
    EXPECT_DOUBLE_EQ(a.get(0, 0), 0.0);
    delete c;
}

TEST(MatrixAdd, Complex) {
    Matrix<Complex<double>> a(1, 1), b(1, 1);
    a.set(0, 0, {1.0, 2.0});
    b.set(0, 0, {3.0, 4.0});
    IMatrix<Complex<double>>* c = a.add(b);
    EXPECT_DOUBLE_EQ(c->get(0, 0).re, 4.0);
    EXPECT_DOUBLE_EQ(c->get(0, 0).im, 6.0);
    delete c;
}

/*******************************************************************
 * mult_scalar
 *******************************************************************/

TEST(MatrixMultScalar, Basic) {
    Matrix<double> m(2, 2);
    fill_seq(m);  // 0 1 2 3
    IMatrix<double>* result = m.mult_scalar(2.0);
    EXPECT_DOUBLE_EQ(result->get(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(result->get(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(result->get(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(result->get(1, 1), 6.0);
    delete result;
}

TEST(MatrixMultScalar, ByZero) {
    Matrix<double> m(2, 2);
    fill_seq(m);
    IMatrix<double>* result = m.mult_scalar(0.0);
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            EXPECT_DOUBLE_EQ(result->get(i, j), 0.0);
    delete result;
}

TEST(MatrixMultScalar, ByOne) {
    Matrix<double> m(2, 2);
    fill_seq(m);
    IMatrix<double>* result = m.mult_scalar(1.0);
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            EXPECT_DOUBLE_EQ(result->get(i, j), m.get(i, j));
    delete result;
}

TEST(MatrixMultScalar, OriginalUnchanged) {
    Matrix<double> m(2, 2);
    fill_seq(m);
    IMatrix<double>* result = m.mult_scalar(5.0);
    EXPECT_DOUBLE_EQ(m.get(0, 0), 0.0);  // m не изменился
    delete result;
}

/*******************************************************************
 * norm (Frobenius)
 *******************************************************************/

TEST(MatrixNorm, ZeroMatrix) {
    Matrix<double> m(3, 3);
    EXPECT_DOUBLE_EQ(m.norm(), 0.0);
}

TEST(MatrixNorm, IdentityMatrix3x3) {
    // norm = sqrt(3)
    Matrix<double> m(3, 3);
    m.set(0, 0, 1.0); m.set(1, 1, 1.0); m.set(2, 2, 1.0);
    EXPECT_NEAR(m.norm(), std::sqrt(3.0), 1e-9);
}

TEST(MatrixNorm, KnownValues) {
    // norm = sqrt(30)
    double data[] = {1.0, 2.0, 3.0, 4.0};
    Matrix<double> m(data, 2, 2);
    EXPECT_NEAR(m.norm(), std::sqrt(30.0), 1e-9);
}

TEST(MatrixNorm, SingleElement) {
    Matrix<double> m(1, 1);
    m.set(0, 0, 5.0);
    EXPECT_NEAR(m.norm(), 5.0, 1e-9);
}

TEST(MatrixNorm, NegativeValues) {
    // sign doesn't matter: sqrt((-3)^2) = 3
    Matrix<double> m(1, 1);
    m.set(0, 0, -3.0);
    EXPECT_NEAR(m.norm(), 3.0, 1e-9);
}

TEST(MatrixNorm, Complex) {
    // sqrt(|1+i|^2) = sqrt(2)
    Matrix<Complex<double>> m(1, 1);
    m.set(0, 0, {1.0, 1.0});
    EXPECT_NEAR(m.norm(), std::sqrt(2.0), 1e-9);
}