// C++20
#include <gtest/gtest.h>
#include "diagonal_matrix.hpp"
#include "square_matrix.hpp"

/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(DiagonalMatrixTest, DefaultConstructor) {
    DiagonalMatrix<double> m;
    EXPECT_EQ(m.get_rows(), 0);
    EXPECT_EQ(m.get_cols(), 0);
    EXPECT_EQ(m.get_size(), 0);
}

TEST(DiagonalMatrixTest, SizeConstructor) {
    DiagonalMatrix<double> m(3);
    EXPECT_EQ(m.get_rows(), 3);
    EXPECT_EQ(m.get_cols(), 3);
}

TEST(DiagonalMatrixTest, SizeConstructorNegativeThrows) {
    EXPECT_THROW(DiagonalMatrix<double>(-1), std::invalid_argument);
}

TEST(DiagonalMatrixTest, ArrayConstructor) {
    double items[] = {1.0, 2.0, 3.0};
    DiagonalMatrix<double> m(items, 3);
    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 2.0);
    EXPECT_DOUBLE_EQ(m.get(2, 2), 3.0);
}

TEST(DiagonalMatrixTest, CopyConstructor) {
    double items[] = {4.0, 5.0};
    DiagonalMatrix<double> original(items, 2);
    DiagonalMatrix<double> copy(original);

    EXPECT_DOUBLE_EQ(copy.get(0, 0), 4.0);
    EXPECT_DOUBLE_EQ(copy.get(1, 1), 5.0);

    // check deep copy
    original.set(0, 0, 99.0);
    EXPECT_DOUBLE_EQ(copy.get(0, 0), 4.0);
}

/*******************************************************************
 * get (ZERO when not diagonal)
 *******************************************************************/

TEST(DiagonalMatrixTest, GetReturnsZeroOffDiagonal) {
    double items[] = {1.0, 2.0, 3.0};
    DiagonalMatrix<double> m(items, 3);

    EXPECT_DOUBLE_EQ(m.get(0, 1), 0.0);
    EXPECT_DOUBLE_EQ(m.get(1, 0), 0.0);
    EXPECT_DOUBLE_EQ(m.get(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(m.get(2, 0), 0.0);
}

TEST(DiagonalMatrixTest, GetOutOfRangeThrows) {
    DiagonalMatrix<double> m(3);
    EXPECT_THROW(m.get(-1, 0), std::out_of_range);
    EXPECT_THROW(m.get(0, -1), std::out_of_range);
    EXPECT_THROW(m.get(3, 0),  std::out_of_range);
    EXPECT_THROW(m.get(0, 3),  std::out_of_range);
}

/*******************************************************************
 * set (throw)
 *******************************************************************/

TEST(DiagonalMatrixTest, SetDiagonalWorks) {
    DiagonalMatrix<double> m(3);
    m.set(1, 1, 7.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 7.0);
}

TEST(DiagonalMatrixTest, SetOffDiagonalThrows) {
    DiagonalMatrix<double> m(3);
    EXPECT_THROW(m.set(0, 1, 5.0), std::invalid_argument);
    EXPECT_THROW(m.set(1, 0, 5.0), std::invalid_argument);
}

TEST(DiagonalMatrixTest, SetOutOfRangeThrows) {
    DiagonalMatrix<double> m(3);
    EXPECT_THROW(m.set(-1, -1, 1.0), std::out_of_range);
    EXPECT_THROW(m.set(3,  3,  1.0), std::out_of_range);
}

/*******************************************************************
 * norm 
 *******************************************************************/

TEST(DiagonalMatrixTest, NormCorrect) {
    double items[] = {3.0, 4.0};
    DiagonalMatrix<double> m(items, 2);
    // sqrt(9 + 16) = 5
    EXPECT_DOUBLE_EQ(m.norm(), 5.0);
}

TEST(DiagonalMatrixTest, NormZeroMatrix) {
    DiagonalMatrix<double> m(3); 
    EXPECT_DOUBLE_EQ(m.norm(), 0.0);
}

/*******************************************************************
 * add
 *******************************************************************/

TEST(DiagonalMatrixTest, AddReturnsSumCorrect) {
    double a_items[] = {1.0, 2.0, 3.0};
    double b_items[] = {4.0, 5.0, 6.0};
    DiagonalMatrix<double> a(a_items, 3);
    DiagonalMatrix<double> b(b_items, 3);

    SquareMatrix<double> *result = a.add(b);

    // diagonal
    EXPECT_DOUBLE_EQ(result->get(0, 0), 5.0);
    EXPECT_DOUBLE_EQ(result->get(1, 1), 7.0);
    EXPECT_DOUBLE_EQ(result->get(2, 2), 9.0);

    // out of diagonal
    EXPECT_DOUBLE_EQ(result->get(0, 1), 0.0);
    EXPECT_DOUBLE_EQ(result->get(1, 2), 0.0);
    
    delete result;
}

TEST(DiagonalMatrixTest, AddSizeMismatchThrows) {
    DiagonalMatrix<double> a(2);
    DiagonalMatrix<double> b(3);
    EXPECT_THROW(a.add(b), std::invalid_argument);
}

/*******************************************************************
 * mult_scalar
 *******************************************************************/

TEST(DiagonalMatrixTest, MultScalarCorrect) {
    double items[] = {1.0, 2.0, 3.0};
    DiagonalMatrix<double> m(items, 3);

    DiagonalMatrix<double> *result = m.mult_scalar(3.0);

    EXPECT_DOUBLE_EQ(result->get(0, 0), 3.0);
    EXPECT_DOUBLE_EQ(result->get(1, 1), 6.0);
    EXPECT_DOUBLE_EQ(result->get(2, 2), 9.0);

    delete result;
}

TEST(DiagonalMatrixTest, MultScalarByZero) {
    double items[] = {1.0, 2.0, 3.0};
    DiagonalMatrix<double> m(items, 3);

    std::unique_ptr<DiagonalMatrix<double>> result(m.mult_scalar(0.0));

    EXPECT_DOUBLE_EQ(result->get(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(result->get(1, 1), 0.0);
    EXPECT_DOUBLE_EQ(result->get(2, 2), 0.0);
}

TEST(DiagonalMatrixTest, MultScalarDoesNotModifyOriginal) {
    double items[] = {1.0, 2.0};
    DiagonalMatrix<double> m(items, 2);

    DiagonalMatrix<double> *result = m.mult_scalar(5.0);

    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 2.0);

    delete result;
}