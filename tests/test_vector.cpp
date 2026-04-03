#include <gtest/gtest.h>
#include <stdexcept>

#include "vector.hpp"

/*******************************************************************
 * Helpers
 *******************************************************************/

template <typename T>
void fill_seq(Vector<T>& v) {
    for (int i = 0; i < v.get_size(); i++)
        v.set(i, static_cast<T>(i));
}

/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(VectorCtor, BasicSize) {
    Vector<double> v(5);
    EXPECT_EQ(v.get_size(), 5);
}

TEST(VectorCtor, AlwaysOneCol) {
    Vector<double> v(4);
    EXPECT_EQ(v.get_cols(), 1);
    EXPECT_EQ(v.get_rows(), 4);
}

TEST(VectorCtor, ZeroSizeValid) {
    EXPECT_NO_THROW(Vector<double>(0));
}

TEST(VectorCtor, NegativeSizeThrows) {
    EXPECT_THROW(Vector<double>(-1), std::invalid_argument);
}

TEST(VectorCtor, DefaultInitialized) {
    Vector<double> v(3);
    for (int i = 0; i < 3; i++)
        EXPECT_DOUBLE_EQ(v[i], 0.0);
}

TEST(VectorCtor, FromRawArray) {
    double data[] = {1.0, 2.0, 3.0};
    Vector<double> v(data, 3);
    EXPECT_DOUBLE_EQ(v[0], 1.0);
    EXPECT_DOUBLE_EQ(v[1], 2.0);
    EXPECT_DOUBLE_EQ(v[2], 3.0);
}

TEST(VectorCtor, CopyIsDeep) {
    Vector<double> a(3);
    fill_seq(a);
    Vector<double> b(a);
    b.set(0, 999.0);
    EXPECT_DOUBLE_EQ(a[0], 0.0);  // a didn't change
}

/*******************************************************************
 * get / set / operator[]
 *******************************************************************/

TEST(VectorAccess, SetAndGet) {
    Vector<double> v(3);
    v.set(1, 42.0);
    EXPECT_DOUBLE_EQ(v.get(1), 42.0);
}

TEST(VectorAccess, GetEqualsOperatorBracket) {
    Vector<double> v(3);
    v.set(2, 7.0);
    EXPECT_DOUBLE_EQ(v.get(2), v[2]);
}

TEST(VectorAccess, GetOutOfRangeThrows) {
    Vector<double> v(3);
    EXPECT_THROW(v.get(-1), std::out_of_range);
    EXPECT_THROW(v.get(3),  std::out_of_range);
}

TEST(VectorAccess, SetOutOfRangeThrows) {
    Vector<double> v(3);
    EXPECT_THROW(v.set(-1, 1.0), std::out_of_range);
    EXPECT_THROW(v.set(3,  1.0), std::out_of_range);
}

TEST(VectorAccess, TwoDimensionalGetStillWorks) {
    // using Matrix<T>::get открывает двумерную версию
    Vector<double> v(3);
    v.set(1, 5.0);
    EXPECT_DOUBLE_EQ(v.Matrix<double>::get(1, 0), 5.0);  // cols is always 0
}

TEST(VectorAccess, TwoDimensionalGetWrongColThrows) {
    Vector<double> v(3);
    EXPECT_THROW(v.Matrix<double>::get(0, 1), std::out_of_range);  // col=1 don't exist
}

/*******************************************************************
 * add — covariant return Vector<T>*
 *******************************************************************/

TEST(VectorAdd, ReturnsVector) {
    Vector<double> a(3), b(3);
    IMatrix<double>* result = a.add(b);
    EXPECT_NE(dynamic_cast<Vector<double>*>(result), nullptr);
    delete result;
}

TEST(VectorAdd, ResultColsIsOne) {
    Vector<double> a(3), b(3);
    Vector<double>* result = a.add(b);
    EXPECT_EQ(result->get_cols(), 1);
    delete result;
}

TEST(VectorAdd, BasicValues) {
    double da[] = {1.0, 2.0, 3.0};
    double db[] = {4.0, 5.0, 6.0};
    Vector<double> a(da, 3), b(db, 3);
    Vector<double>* result = a.add(b);
    EXPECT_DOUBLE_EQ((*result)[0], 5.0);
    EXPECT_DOUBLE_EQ((*result)[1], 7.0);
    EXPECT_DOUBLE_EQ((*result)[2], 9.0);
    delete result;
}

TEST(VectorAdd, AddZeroVector) {
    double data[] = {1.0, 2.0, 3.0};
    Vector<double> a(data, 3), zero(3);
    Vector<double>* result = a.add(zero);
    for (int i = 0; i < 3; i++)
        EXPECT_DOUBLE_EQ((*result)[i], a[i]);
    delete result;
}

TEST(VectorAdd, SizeMismatchThrows) {
    Vector<double> a(3), b(4);
    EXPECT_THROW(a.add(b), std::invalid_argument);
}

TEST(VectorAdd, OriginalUnchanged) {
    double data[] = {1.0, 2.0, 3.0};
    Vector<double> a(data, 3), b(3);
    Vector<double>* result = a.add(b);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    delete result;
}

TEST(VectorAdd, WithIMatrix) {
    // add takes IMatrix 
    double da[] = {1.0, 2.0, 3.0};
    double db[] = {1.0, 2.0, 3.0};
    Vector<double> a(da, 3);
    Matrix<double> b(db, 3, 1);  // 3×1, but Matrix
    Vector<double>* result = a.add(b);
    EXPECT_DOUBLE_EQ((*result)[0], 2.0);
    delete result;
}

/*******************************************************************
 * mult_scalar — covariant return Vector<T>*
 *******************************************************************/

TEST(VectorMultScalar, ReturnsVector) {
    Vector<double> v(3);
    IMatrix<double>* result = v.mult_scalar(2.0);
    EXPECT_NE(dynamic_cast<Vector<double>*>(result), nullptr);
    delete result;
}

TEST(VectorMultScalar, BasicValues) {
    double data[] = {1.0, 2.0, 3.0};
    Vector<double> v(data, 3);
    Vector<double>* result = v.mult_scalar(3.0);
    EXPECT_DOUBLE_EQ((*result)[0], 3.0);
    EXPECT_DOUBLE_EQ((*result)[1], 6.0);
    EXPECT_DOUBLE_EQ((*result)[2], 9.0);
    delete result;
}

TEST(VectorMultScalar, ByZero) {
    double data[] = {1.0, 2.0, 3.0};
    Vector<double> v(data, 3);
    Vector<double>* result = v.mult_scalar(0.0);
    for (int i = 0; i < 3; i++)
        EXPECT_DOUBLE_EQ((*result)[i], 0.0);
    delete result;
}

TEST(VectorMultScalar, ByOne) {
    double data[] = {4.0, 5.0, 6.0};
    Vector<double> v(data, 3);
    Vector<double>* result = v.mult_scalar(1.0);
    for (int i = 0; i < 3; i++)
        EXPECT_DOUBLE_EQ((*result)[i], v[i]);
    delete result;
}

TEST(VectorMultScalar, OriginalUnchanged) {
    double data[] = {1.0, 2.0, 3.0};
    Vector<double> v(data, 3);
    Vector<double>* result = v.mult_scalar(5.0);
    EXPECT_DOUBLE_EQ(v[0], 1.0);
    delete result;
}

/*******************************************************************
 * norm 
 *******************************************************************/

TEST(VectorNorm, ZeroVector) {
    Vector<double> v(3);
    EXPECT_DOUBLE_EQ(v.norm(), 0.0);
}

TEST(VectorNorm, KnownValues) {
    // [3, 4] → norm = 5
    double data[] = {3.0, 4.0};
    Vector<double> v(data, 2);
    EXPECT_NEAR(v.norm(), 5.0, 1e-9);
}

TEST(VectorNorm, UnitVector) {
    double data[] = {1.0, 0.0, 0.0};
    Vector<double> v(data, 3);
    EXPECT_NEAR(v.norm(), 1.0, 1e-9);
}