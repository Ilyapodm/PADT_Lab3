#include <gtest/gtest.h>
#include "sparse_matrix.hpp"

/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(SparseMatrix, DefaultConstructor) {
    SparseMatrix<double> m;
    EXPECT_EQ(m.get_rows(), 0);
    EXPECT_EQ(m.get_cols(), 0);
    EXPECT_EQ(m.get_data_size(), 0);
}

TEST(SparseMatrix, SizeConstructor) {
    SparseMatrix<double> m(3, 4);
    EXPECT_EQ(m.get_rows(), 3);
    EXPECT_EQ(m.get_cols(), 4);
    EXPECT_EQ(m.get_data_size(), 0);
}

TEST(SparseMatrix, NegativeSizeThrows) {
    EXPECT_THROW(SparseMatrix<double>(-1, 4), std::invalid_argument);
    EXPECT_THROW(SparseMatrix<double>(4, -1), std::invalid_argument);
}

TEST(SparseMatrix, CopyConstructor) {
    SparseMatrix<double> a(3, 3);
    a.set(0, 1, 5.0);
    a.set(2, 2, 7.0);

    SparseMatrix<double> b(a);
    EXPECT_DOUBLE_EQ(b.get(0, 1), 5.0);
    EXPECT_DOUBLE_EQ(b.get(2, 2), 7.0);
    EXPECT_EQ(b.get_data_size(), 2);
}

TEST(SparseMatrix, CopyIsIndependent) {
    SparseMatrix<double> a(3, 3);
    a.set(0, 0, 1.0);

    SparseMatrix<double> b(a);
    b.set(0, 0, 99.0);

    EXPECT_DOUBLE_EQ(a.get(0, 0), 1.0); // a didn't changed
}

/*******************************************************************
 * get / set - basics
 *******************************************************************/

TEST(SparseMatrix, GetReturnsZeroForEmpty) {
    SparseMatrix<double> m(5, 5);
    EXPECT_DOUBLE_EQ(m.get(2, 3), 0.0);
}

TEST(SparseMatrix, SetAndGet) {
    SparseMatrix<double> m(5, 5);
    m.set(1, 3, 42.0);
    EXPECT_DOUBLE_EQ(m.get(1, 3), 42.0);
    EXPECT_EQ(m.get_data_size(), 1);
}

TEST(SparseMatrix, SetMultipleAndGet) {
    SparseMatrix<double> m(5, 5);
    m.set(0, 0, 1.0);
    m.set(2, 4, 2.0);
    m.set(1, 1, 3.0);

    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 3.0);
    EXPECT_DOUBLE_EQ(m.get(2, 4), 2.0);
    EXPECT_EQ(m.get_data_size(), 3);
}

TEST(SparseMatrix, GetOutOfRangeThrows) {
    SparseMatrix<double> m(3, 3);
    EXPECT_THROW(m.get(-1, 0), std::out_of_range);
    EXPECT_THROW(m.get(0, 3),  std::out_of_range);
    EXPECT_THROW(m.get(3, 0),  std::out_of_range);
}

TEST(SparseMatrix, SetOutOfRangeThrows) {
    SparseMatrix<double> m(3, 3);
    EXPECT_THROW(m.set(-1, 0, 1.0), std::out_of_range);
    EXPECT_THROW(m.set(0,  3, 1.0), std::out_of_range);
}

/*******************************************************************
 * get / set - updating and clearing
 *******************************************************************/

TEST(SparseMatrix, SetOverwritesExisting) {
    SparseMatrix<double> m(4, 4);
    m.set(1, 2, 10.0);
    m.set(1, 2, 20.0);
    EXPECT_DOUBLE_EQ(m.get(1, 2), 20.0);
    EXPECT_EQ(m.get_data_size(), 1); // new triplet didn't add
}

TEST(SparseMatrix, SetZeroRemovesElement) {
    SparseMatrix<double> m(4, 4);
    m.set(1, 2, 10.0);
    m.set(1, 2, 0.0);
    EXPECT_DOUBLE_EQ(m.get(1, 2), 0.0);
    EXPECT_EQ(m.get_data_size(), 0);
}

TEST(SparseMatrix, SetZeroOnEmptyDoesNothing) {
    SparseMatrix<double> m(4, 4);
    m.set(0, 0, 0.0);
    EXPECT_EQ(m.get_data_size(), 0);
}

/*******************************************************************
 * sort of triplets
 *******************************************************************/

TEST(SparseMatrix, InsertReverseOrderStillCorrect) {
    SparseMatrix<double> m(3, 3);
    m.set(2, 2, 3.0);
    m.set(0, 0, 1.0);
    m.set(1, 1, 2.0);

    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 2.0);
    EXPECT_DOUBLE_EQ(m.get(2, 2), 3.0);
}

/*******************************************************************
 * add
 *******************************************************************/

TEST(SparseMatrix, AddBasic) {
    SparseMatrix<double> a(3, 3);
    SparseMatrix<double> b(3, 3);
    a.set(0, 0, 1.0);
    a.set(1, 2, 3.0);
    b.set(0, 0, 2.0);
    b.set(2, 1, 4.0);

    SparseMatrix<double>* result = a.add(b);
    EXPECT_DOUBLE_EQ(result->get(0, 0), 3.0);
    EXPECT_DOUBLE_EQ(result->get(1, 2), 3.0);
    EXPECT_DOUBLE_EQ(result->get(2, 1), 4.0);
    EXPECT_DOUBLE_EQ(result->get(2, 2), 0.0);
    delete result;
}

TEST(SparseMatrix, AddOppositesCancelToZero) {
    SparseMatrix<double> a(2, 2);
    SparseMatrix<double> b(2, 2);
    a.set(0, 0,  5.0);
    b.set(0, 0, -5.0);

    SparseMatrix<double>* result = a.add(b);
    EXPECT_DOUBLE_EQ(result->get(0, 0), 0.0);
    EXPECT_EQ(result->get_data_size(), 0);
    delete result;
}

TEST(SparseMatrix, AddSizeMismatchThrows) {
    SparseMatrix<double> a(2, 3);
    SparseMatrix<double> b(3, 2);
    EXPECT_THROW(a.add(b), std::invalid_argument);
}

/*******************************************************************
 * mult_scalar
 *******************************************************************/

TEST(SparseMatrix, MultScalarBasic) {
    SparseMatrix<double> m(3, 3);
    m.set(0, 1, 4.0);
    m.set(2, 2, 6.0);

    SparseMatrix<double>* result = m.mult_scalar(3.0);
    EXPECT_DOUBLE_EQ(result->get(0, 1), 12.0);
    EXPECT_DOUBLE_EQ(result->get(2, 2), 18.0);
    EXPECT_EQ(result->get_data_size(), 2);
    delete result;
}

TEST(SparseMatrix, MultScalarByZeroReturnsEmpty) {
    SparseMatrix<double> m(3, 3);
    m.set(0, 0, 5.0);
    m.set(1, 1, 3.0);

    SparseMatrix<double>* result = m.mult_scalar(0.0);
    EXPECT_EQ(result->get_data_size(), 0);
    delete result;
}

TEST(SparseMatrix, MultScalarByOne) {
    SparseMatrix<double> m(2, 2);
    m.set(0, 0, 7.0);

    SparseMatrix<double>* result = m.mult_scalar(1.0);
    EXPECT_DOUBLE_EQ(result->get(0, 0), 7.0);
    delete result;
}

/*******************************************************************
 * norm
 *******************************************************************/

TEST(SparseMatrix, NormEmpty) {
    SparseMatrix<double> m(3, 3);
    EXPECT_DOUBLE_EQ(m.norm(), 0.0);
}

TEST(SparseMatrix, NormBasic) {
    SparseMatrix<double> m(3, 3);
    m.set(0, 0, 3.0);
    m.set(1, 1, 4.0);
    // sqrt(9 + 16) = 5
    EXPECT_DOUBLE_EQ(m.norm(), 5.0);
}