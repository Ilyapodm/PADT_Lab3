#include <gtest/gtest.h>
#include "system_of_equations.hpp"

using T   = double;
using SOE = SystemOfEquations<T>;

static const double EPS    = 1e-8;   // for solution components
static const double EPS_R  = 1e-10;  // for residual on well-conditioned systems

/*******************************************************************
 * helpers
 *******************************************************************/

// Build a 2x2 system: A=[[2,1],[1,3]], b=[5,10], x_exact=[1,3]
static SOE make_2x2() {
    SquareMatrix<T> A(2);
    A.set(0,0,2); A.set(0,1,1);
    A.set(1,0,1); A.set(1,1,3);
    Vector<T> b(2);
    b.set(0,5); b.set(1,10);
    return SOE(A, b);
}

// Build a 3x3 system: x_exact=[2,3,-1]
static SOE make_3x3() {
    SquareMatrix<T> A(3);
    A.set(0,0, 2); A.set(0,1, 1); A.set(0,2,-1);
    A.set(1,0,-3); A.set(1,1,-1); A.set(1,2, 2);
    A.set(2,0,-2); A.set(2,1, 1); A.set(2,2, 2);
    Vector<T> b(3);
    b.set(0, 8); b.set(1,-11); b.set(2,-3);
    return SOE(A, b);
}

// Build a singular 2x2: rows are linearly dependent
static SOE make_singular() {
    SquareMatrix<T> A(2);
    A.set(0,0,1); A.set(0,1,2);
    A.set(1,0,2); A.set(1,1,4);
    Vector<T> b(2);
    b.set(0,3); b.set(1,6);
    return SOE(A, b);
}

/*******************************************************************
 * constructors and getters
 *******************************************************************/

TEST(SOE_Getters, SizeIsCorrect) {
    SOE soe = make_2x2();
    EXPECT_EQ(soe.get_size(), 2);

    SOE soe3 = make_3x3();
    EXPECT_EQ(soe3.get_size(), 3);
}

TEST(SOE_Getters, GetMatrixReturnsA) {
    SOE soe = make_2x2();
    EXPECT_DOUBLE_EQ(soe.get_matrix().get(0,0), 2.0);
    EXPECT_DOUBLE_EQ(soe.get_matrix().get(0,1), 1.0);
    EXPECT_DOUBLE_EQ(soe.get_matrix().get(1,0), 1.0);
    EXPECT_DOUBLE_EQ(soe.get_matrix().get(1,1), 3.0);
}

TEST(SOE_Getters, GetRhsReturnsB) {
    SOE soe = make_2x2();
    EXPECT_DOUBLE_EQ(soe.get_rhs().get(0), 5.0);
    EXPECT_DOUBLE_EQ(soe.get_rhs().get(1), 10.0);
}

/*******************************************************************
 * setters
 *******************************************************************/

TEST(SOE_Setters, SetRhsChangesB) {
    SOE soe = make_2x2();
    Vector<T> b2(2);
    b2.set(0, 99.0); b2.set(1, 77.0);
    soe.set_rhs(b2);
    EXPECT_DOUBLE_EQ(soe.get_rhs().get(0), 99.0);
    EXPECT_DOUBLE_EQ(soe.get_rhs().get(1), 77.0);
}

TEST(SOE_Setters, SetMatrixChangesA) {
    SOE soe = make_2x2();
    SquareMatrix<T> A2(2);
    A2.set(0,0,5); A2.set(0,1,0);
    A2.set(1,0,0); A2.set(1,1,5);
    soe.set_matrix(A2);
    EXPECT_DOUBLE_EQ(soe.get_matrix().get(0,0), 5.0);
    EXPECT_DOUBLE_EQ(soe.get_matrix().get(1,1), 5.0);
}

TEST(SOE_Setters, SetReplacesBoth) {
    SOE soe = make_2x2();
    SOE soe3 = make_3x3();
    soe.set(soe3.get_matrix(), soe3.get_rhs());
    EXPECT_EQ(soe.get_size(), 3);
    EXPECT_DOUBLE_EQ(soe.get_matrix().get(0,0), 2.0);
    EXPECT_DOUBLE_EQ(soe.get_rhs().get(1), -11.0);
}

// ============================================================
// solve_gauss
// ============================================================

TEST(SOE_Gauss, Solves2x2) {
    SOE soe = make_2x2();
    Vector<T>* x = soe.solve_gauss();
    EXPECT_NEAR(x->get(0), 1.0, EPS);
    EXPECT_NEAR(x->get(1), 3.0, EPS);
    delete x;
}

TEST(SOE_Gauss, Solves3x3) {
    SOE soe = make_3x3();
    Vector<T>* x = soe.solve_gauss();
    EXPECT_NEAR(x->get(0),  2.0, EPS);
    EXPECT_NEAR(x->get(1),  3.0, EPS);
    EXPECT_NEAR(x->get(2), -1.0, EPS);
    delete x;
}

TEST(SOE_Gauss, ThrowsOnSingular) {
    SOE soe = make_singular();
    EXPECT_THROW(soe.solve_gauss(), std::runtime_error);
}

// ============================================================
// solve_gauss_with_pivot
// ============================================================

TEST(SOE_GaussPivot, Solves2x2) {
    SOE soe = make_2x2();
    Vector<T>* x = soe.solve_gauss_with_pivot();
    EXPECT_NEAR(x->get(0), 1.0, EPS);
    EXPECT_NEAR(x->get(1), 3.0, EPS);
    delete x;
}

TEST(SOE_GaussPivot, Solves3x3) {
    SOE soe = make_3x3();
    Vector<T>* x = soe.solve_gauss_with_pivot();
    EXPECT_NEAR(x->get(0),  2.0, EPS);
    EXPECT_NEAR(x->get(1),  3.0, EPS);
    EXPECT_NEAR(x->get(2), -1.0, EPS);
    delete x;
}

TEST(SOE_GaussPivot, ThrowsOnSingular) {
    SOE soe = make_singular();
    EXPECT_THROW(soe.solve_gauss_with_pivot(), std::runtime_error);
}

// ============================================================
// solve_plu
// ============================================================

TEST(SOE_PLU, Solves2x2) {
    SOE soe = make_2x2();
    Vector<T>* x = soe.solve_plu();
    EXPECT_NEAR(x->get(0), 1.0, EPS);
    EXPECT_NEAR(x->get(1), 3.0, EPS);
    delete x;
}

TEST(SOE_PLU, Solves3x3) {
    SOE soe = make_3x3();
    Vector<T>* x = soe.solve_plu();
    EXPECT_NEAR(x->get(0),  2.0, EPS);
    EXPECT_NEAR(x->get(1),  3.0, EPS);
    EXPECT_NEAR(x->get(2), -1.0, EPS);
    delete x;
}

TEST(SOE_PLU, ThrowsOnSingular) {
    SOE soe = make_singular();
    EXPECT_THROW(soe.solve_plu(), std::runtime_error);
}

// PLU cache: set_rhs does NOT reset lu_ready
// Solve twice with different b — second call reuses decomposition
TEST(SOE_PLU, CacheReusedAfterSetRhs) {
    SOE soe = make_2x2();

    // first solve: triggers decompose
    Vector<T>* x1 = soe.solve_plu();
    EXPECT_NEAR(x1->get(0), 1.0, EPS);
    EXPECT_NEAR(x1->get(1), 3.0, EPS);
    delete x1;

    // change b to [3, 7] → x_exact = [0.8, 1.4]
    // 2*0.8 + 1*1.4 = 3 ✓   1*0.8 + 3*1.4 = 5 ≠ 7...
    // Let me use b=[4,9]: 2x+y=4, x+3y=9 → x=3/5, y=14/5... messy
    // Use b=[7,13]: 2*2+1*3=7 ✓, 1*2+3*3+2=... let x=[2,3], b=[7,11]
    // 2*2+1*3=7 ✓, 1*2+3*3=11 ✓
    Vector<T> b2(2);
    b2.set(0, 7.0); b2.set(1, 11.0);
    soe.set_rhs(b2);  // must NOT reset lu_ready

    // second solve: only substitution, no re-decompose
    Vector<T>* x2 = soe.solve_plu();
    EXPECT_NEAR(x2->get(0), 2.0, EPS);
    EXPECT_NEAR(x2->get(1), 3.0, EPS);
    delete x2;
}

// set_matrix RESETS lu_ready — next solve_plu must re-decompose correctly
TEST(SOE_PLU, CacheResetAfterSetMatrix) {
    SOE soe = make_2x2();

    Vector<T>* x1 = soe.solve_plu();  // decompose for [[2,1],[1,3]]
    delete x1;

    // swap to identity matrix, b=[5,7] → x=[5,7]
    SquareMatrix<T> I(2);
    I.set(0,0,1); I.set(0,1,0);
    I.set(1,0,0); I.set(1,1,1);
    Vector<T> b2(2);
    b2.set(0,5); b2.set(1,7);
    soe.set(I, b2);  // resets cache

    Vector<T>* x2 = soe.solve_plu();
    EXPECT_NEAR(x2->get(0), 5.0, EPS);
    EXPECT_NEAR(x2->get(1), 7.0, EPS);
    delete x2;
}

// ============================================================
// All three methods agree
// ============================================================

TEST(SOE_Consistency, AllMethodsAgreeOn3x3) {
    SOE soe = make_3x3();

    Vector<T>* xg  = soe.solve_gauss();
    Vector<T>* xgp = soe.solve_gauss_with_pivot();
    Vector<T>* xlu = soe.solve_plu();

    for (int i = 0; i < 3; i++) {
        EXPECT_NEAR(xg->get(i), xgp->get(i), EPS);
        EXPECT_NEAR(xg->get(i), xlu->get(i), EPS);
    }
    delete xg; delete xgp; delete xlu;
}

// ============================================================
// Metrics
// ============================================================

TEST(SOE_Metrics, ResidualNearZeroForExactSolution) {
    SOE soe = make_3x3();
    Vector<T>* x = soe.solve_plu();
    EXPECT_NEAR(soe.residual(*x), 0.0, EPS_R);
    delete x;
}

TEST(SOE_Metrics, RelativeErrorZeroForExactSolution) {
    Vector<T> approx(3), exact(3);
    approx.set(0, 2.0); approx.set(1, 3.0); approx.set(2,-1.0);
    exact.set(0,  2.0); exact.set(1, 3.0);  exact.set(2,-1.0);
    EXPECT_NEAR(SOE::relative_error(approx, exact), 0.0, EPS_R);
}

TEST(SOE_Metrics, RelativeErrorNonZeroForBadSolution) {
    Vector<T> approx(2), exact(2);
    approx.set(0, 1.1); approx.set(1, 3.0);
    exact.set(0,  1.0); exact.set(1, 3.0);
    EXPECT_GT(SOE::relative_error(approx, exact), 0.0);
}

// ============================================================
// Factories
// ============================================================

TEST(SOE_Factories, RandomHasCorrectSize) {
    SOE soe = SOE::random(5, 42);
    EXPECT_EQ(soe.get_size(), 5);
}

TEST(SOE_Factories, RandomIsSolvable) {
    // random with seed=42 should be non-singular (guaranteed by construction)
    SOE soe = SOE::random(10, 42);
    Vector<T>* x = soe.solve_plu();
    EXPECT_NEAR(soe.residual(*x), 0.0, EPS_R);
    delete x;
}

TEST(SOE_Factories, RandomSameSeedGivesSameResult) {
    SOE soe1 = SOE::random(5, 42);
    SOE soe2 = SOE::random(5, 42);
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            EXPECT_DOUBLE_EQ(soe1.get_matrix().get(i,j),
                             soe2.get_matrix().get(i,j));
}

TEST(SOE_Factories, HilbertHasCorrectSize) {
    SOE soe = SOE::hilbert(5);
    EXPECT_EQ(soe.get_size(), 5);
}

TEST(SOE_Factories, HilbertSolutionIsOnes) {
    // hilbert() sets b = A * [1,...,1] — exact solution must be all ones
    SOE soe = SOE::hilbert(5);
    Vector<T>* x = soe.solve_plu();
    for (int i = 0; i < 5; i++)
        EXPECT_NEAR(x->get(i), 1.0, 1e-6);  // looser tol — Hilbert is ill-conditioned
    delete x;
}

TEST(SOE_Factories, HilbertResidualIsSmall) {
    SOE soe = SOE::hilbert(5);
    Vector<T>* x = soe.solve_plu();
    EXPECT_LT(soe.residual(*x), 1e-8);
    delete x;
}