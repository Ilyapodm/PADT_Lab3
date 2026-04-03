#include <gtest/gtest.h>
#include "triangular_matrix.hpp"

/*******************************************************************
 * utils
 *******************************************************************/

// 3×3 lower:  | 1 0 0 |
//             | 2 3 0 |
//             | 4 5 6 |
static TriangularMatrix<double> make_lower_3x3() {
    TriangularMatrix<double> m(3, TriangularMatrix<double>::Kind::Lower);
    m.set(0, 0, 1); 
    m.set(1, 0, 2); m.set(1, 1, 3);
    m.set(2, 0, 4); m.set(2, 1, 5); m.set(2, 2, 6);
    return m;
}

// 3×3 upper:  | 1 2 3 |
//             | 0 4 5 |
//             | 0 0 6 |
static TriangularMatrix<double> make_upper_3x3() {
    TriangularMatrix<double> m(3, TriangularMatrix<double>::Kind::Upper);
    m.set(0, 0, 1); m.set(0, 1, 2); m.set(0, 2, 3);
                    m.set(1, 1, 4); m.set(1, 2, 5);
                                    m.set(2, 2, 6);
    return m;
}

/*******************************************************************
 * constructor
 *******************************************************************/

TEST(TriangularMatrix, DefaultConstructorZeroSize) {
    TriangularMatrix<double> m;
    EXPECT_EQ(m.get_rows(), 0);
    EXPECT_EQ(m.get_cols(), 0);
}

TEST(TriangularMatrix, ConstructionSetsSize) {
    TriangularMatrix<double> m(5, TriangularMatrix<double>::Kind::Lower);
    EXPECT_EQ(m.get_rows(), 5);
    EXPECT_EQ(m.get_cols(), 5);
    EXPECT_EQ(m.get_size(), 5);
}

TEST(TriangularMatrix, NegativeSizeThrows) {
    EXPECT_THROW(
        TriangularMatrix<double>(-1, TriangularMatrix<double>::Kind::Lower),
        std::invalid_argument
    );
}

TEST(TriangularMatrix, NewMatrixIsZeroInitialized) {
    TriangularMatrix<double> m(3, TriangularMatrix<double>::Kind::Lower);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            EXPECT_DOUBLE_EQ(m.get(i, j), 0.0);
}

/*******************************************************************
 * getters
 *******************************************************************/

TEST(TriangularMatrix, LowerGetInsideTriangle) {
    auto m = make_lower_3x3();
    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(1, 0), 2.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 3.0);
    EXPECT_DOUBLE_EQ(m.get(2, 0), 4.0);
    EXPECT_DOUBLE_EQ(m.get(2, 1), 5.0);
    EXPECT_DOUBLE_EQ(m.get(2, 2), 6.0);
}

TEST(TriangularMatrix, LowerGetOutsideTriangleIsZero) {
    auto m = make_lower_3x3();
    EXPECT_DOUBLE_EQ(m.get(0, 1), 0.0);  // upper triangle
    EXPECT_DOUBLE_EQ(m.get(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(m.get(1, 2), 0.0);
}

TEST(TriangularMatrix, UpperGetInsideTriangle) {
    auto m = make_upper_3x3();
    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(m.get(0, 2), 3.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 4.0);
    EXPECT_DOUBLE_EQ(m.get(1, 2), 5.0);
    EXPECT_DOUBLE_EQ(m.get(2, 2), 6.0);
}

TEST(TriangularMatrix, UpperGetOutsideTriangleIsZero) {
    auto m = make_upper_3x3();
    EXPECT_DOUBLE_EQ(m.get(1, 0), 0.0);  // lower triangle
    EXPECT_DOUBLE_EQ(m.get(2, 0), 0.0);
    EXPECT_DOUBLE_EQ(m.get(2, 1), 0.0);
}

TEST(TriangularMatrix, GetOutOfBoundsThrows) {
    auto m = make_lower_3x3();
    EXPECT_THROW(m.get(-1, 0), std::out_of_range);
    EXPECT_THROW(m.get(0, -1), std::out_of_range);
    EXPECT_THROW(m.get(3,  0), std::out_of_range);
    EXPECT_THROW(m.get(0,  3), std::out_of_range);
}

/*******************************************************************
 * setters
 *******************************************************************/

TEST(TriangularMatrix, SetOutOfBoundsThrows) {
    TriangularMatrix<double> m(3, TriangularMatrix<double>::Kind::Lower);
    EXPECT_THROW(m.set(-1, 0, 1.0), std::out_of_range);
    EXPECT_THROW(m.set(0, -1, 1.0), std::out_of_range);
    EXPECT_THROW(m.set(3,  0, 1.0), std::out_of_range);
}

TEST(TriangularMatrix, LowerSetOutsideTriangleThrows) {
    TriangularMatrix<double> m(3, TriangularMatrix<double>::Kind::Lower);
    EXPECT_THROW(m.set(0, 1, 99.0), std::invalid_argument);  // upper zone
    EXPECT_THROW(m.set(0, 2, 99.0), std::invalid_argument);
    EXPECT_THROW(m.set(1, 2, 99.0), std::invalid_argument);
}

TEST(TriangularMatrix, UpperSetOutsideTriangleThrows) {
    TriangularMatrix<double> m(3, TriangularMatrix<double>::Kind::Upper);
    EXPECT_THROW(m.set(1, 0, 99.0), std::invalid_argument);  // lower zone
    EXPECT_THROW(m.set(2, 0, 99.0), std::invalid_argument);
    EXPECT_THROW(m.set(2, 1, 99.0), std::invalid_argument);
}

/*******************************************************************
 * norm
 *******************************************************************/

TEST(TriangularMatrix, NormLower) {
    auto m = make_lower_3x3();
    // sqrt(1^2 + 2^2 + 3^2 + 4^2 + 5^2 + 6^2) = sqrt(91)
    EXPECT_NEAR(m.norm(), std::sqrt(91.0), 1e-9);
}

TEST(TriangularMatrix, NormUpper) {
    auto m = make_upper_3x3();
    EXPECT_NEAR(m.norm(), std::sqrt(91.0), 1e-9);
}

TEST(TriangularMatrix, NormZeroMatrix) {
    TriangularMatrix<double> m(4, TriangularMatrix<double>::Kind::Lower);
    EXPECT_DOUBLE_EQ(m.norm(), 0.0);
}

/*******************************************************************
 * mult_scalar
 *******************************************************************/

TEST(TriangularMatrix, MultScalarLower) {
    auto m = make_lower_3x3();
    auto* r = m.mult_scalar(2.0);

    EXPECT_DOUBLE_EQ(r->get(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(r->get(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(r->get(2, 2), 12.0);
    // zero is zero
    EXPECT_DOUBLE_EQ(r->get(0, 1), 0.0);

    delete r;
}

TEST(TriangularMatrix, MultScalarPreservesKind) {
    auto lower = make_lower_3x3();
    auto* r = lower.mult_scalar(3.0);
    EXPECT_DOUBLE_EQ(r->get(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(r->get(2, 0), 12.0);
    delete r;
}

TEST(TriangularMatrix, MultScalarByZero) {
    auto m = make_lower_3x3();
    auto* r = m.mult_scalar(0.0);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            EXPECT_DOUBLE_EQ(r->get(i, j), 0.0);
    delete r;
}

/*******************************************************************
 * add
 *******************************************************************/

TEST(TriangularMatrix, AddLowerPlusLower) {
    auto a = make_lower_3x3();
    auto b = make_lower_3x3();
    auto* r = a.add(b);

    // each has doubled
    EXPECT_DOUBLE_EQ(r->get(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(r->get(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(r->get(2, 2), 12.0);
    // zero is zero
    EXPECT_DOUBLE_EQ(r->get(0, 1), 0.0);

    delete r;
}

TEST(TriangularMatrix, AddLowerPlusUpper) {
    auto lower = make_lower_3x3();
    auto upper = make_upper_3x3();
    auto* r = lower.add(upper);

    // [0][0]: 1+1=2, [0][1]: 0+2=2, [1][0]: 2+0=2, [2][2]: 6+6=12
    EXPECT_DOUBLE_EQ(r->get(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(r->get(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(r->get(1, 0), 2.0);
    EXPECT_DOUBLE_EQ(r->get(2, 2), 12.0);

    delete r;
}

TEST(TriangularMatrix, AddSizeMismatchThrows) {
    auto a = make_lower_3x3();
    TriangularMatrix<double> b(4, TriangularMatrix<double>::Kind::Lower);
    EXPECT_THROW(a.add(b), std::invalid_argument);
}