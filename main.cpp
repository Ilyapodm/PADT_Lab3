#include <iostream>
#include <chrono>
#include <iomanip>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>

#include "math_types.hpp"
#include "imatrix.hpp"
#include "matrix.hpp"
#include "square_matrix.hpp"
#include "vector.hpp"
#include "triangular_matrix.hpp"
#include "diagonal_matrix.hpp"
#include "system_of_equations.hpp"
#include "array_sequence.hpp"

//TODO Разобраться в багах в эксперименте
//TODO потыкать еще и посмотреть на фолбэки
// TODO оформить эксперимент в таблицу
// TODO ux в main (clear, больше менюё)
// TODO сделать sparse matrix

// ============================================================
// Types
// ============================================================

using T   = double;
using Seq = MutableArraySequence<IMatrix<T>*>;

// ============================================================
// Input helpers
// ============================================================

static void clear_input() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int read_int(const std::string& prompt) {
    int val;
    for (;;) {
        std::cout << prompt;
        if (std::cin >> val) return val;
        std::cout << "  [!] Invalid input, try again.\n";
        clear_input();
    }
}

static double read_double(const std::string& prompt) {
    double val;
    for (;;) {
        std::cout << prompt;
        if (std::cin >> val) return val;
        std::cout << "  [!] Invalid input, try again.\n";
        clear_input();
    }
}

static int read_valid_idx(const Seq& seq) {
    for (;;) {
        int idx = read_int("  Matrix index: ");
        if (idx >= 0 && idx < seq.get_size()) return idx;
        std::cout << "  [!] Index out of range [0, " << seq.get_size() - 1 << "].\n";
    }
}

// ============================================================
// Matrix helpers
// ============================================================

static void show_all(const Seq& seq) {
    if (seq.get_size() == 0) { std::cout << "  (empty)\n"; return; }
    for (int i = 0; i < seq.get_size(); i++) {
        const IMatrix<T>* m = seq.get(i);
        std::cout << "  [" << i << "]  "
                  << m->get_rows() << "x" << m->get_cols()
                  << "  norm = " << std::fixed << std::setprecision(4) << m->norm()
                  << "\n";
    }
}

static void fill_manually(IMatrix<T>* m) {
    std::cout << "  Enter elements row by row:\n";
    for (int i = 0; i < m->get_rows(); i++)
        for (int j = 0; j < m->get_cols(); j++) {
            double v = read_double("    [" + std::to_string(i) + "][" + std::to_string(j) + "] = ");
            try { m->set(i, j, v); }
            catch (const std::invalid_argument&) {
                std::cout << "    [!] Position (" << i << "," << j << ") outside structure, skipped.\n";
            }
        }
}

static void fill_randomly(IMatrix<T>* m, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    for (int i = 0; i < m->get_rows(); i++)
        for (int j = 0; j < m->get_cols(); j++) {
            try { m->set(i, j, AlgebraTraits<T>::random_value(rng, dist)); }
            catch (const std::invalid_argument&) {}  // skip out-of-structure positions
        }
}

// ============================================================
// Matrix creation
// ============================================================

static IMatrix<T>* create_matrix() {
    std::cout << "\n  Matrix type:\n"
              << "    1. Matrix (rectangular dense)\n"
              << "    2. SquareMatrix\n"
              << "    3. DiagonalMatrix\n"
              << "    4. TriangularMatrix Lower\n"
              << "    5. TriangularMatrix Upper\n"
              << "    6. SparseMatrix [not implemented]\n";
    int type = read_int("  Choice: ");
    if (type == 6) { std::cout << "  [!] SparseMatrix not implemented yet.\n"; return nullptr; }

    IMatrix<T>* m = nullptr;
    switch (type) {
        case 1: m = new Matrix<T>(read_int("  Rows: "), read_int("  Cols: ")); break;
        case 2: m = new SquareMatrix<T>(read_int("  Size n: ")); break;
        case 3: m = new DiagonalMatrix<T>(read_int("  Size n: ")); break;
        case 4: m = new TriangularMatrix<T>(read_int("  Size n: "), TriangularMatrix<T>::Kind::Lower); break;
        case 5: m = new TriangularMatrix<T>(read_int("  Size n: "), TriangularMatrix<T>::Kind::Upper); break;
        default: std::cout << "  [!] Unknown type.\n"; return nullptr;
    }

    std::cout << "  Fill: 1. Manual  2. Random\n";
    if (read_int("  Choice: ") == 1)
        fill_manually(m);
    else
        fill_randomly(m, static_cast<unsigned>(read_int("  Seed: ")));

    return m;
}

// ============================================================
// Operations on one matrix
// ============================================================

static void matrix_ops(Seq& seq, int idx) {
    for (;;) {
        IMatrix<T>* m = seq.get(idx);
        std::cout << "\n--- Matrix [" << idx << "] " << m->get_rows() << "x" << m->get_cols() << " ---\n"
                  << "  1. Print\n"
                  << "  2. get(i, j)\n"
                  << "  3. set(i, j, val)\n"
                  << "  4. norm()\n"
                  << "  5. add(other)       -> save result\n"
                  << "  6. mult_scalar(val) -> save result\n"
                  << "  0. Back\n";
        int op = read_int("  Choice: ");
        if (op == 0) return;

        switch (op) {
            case 1:
                std::cout << *m;
                break;
            case 2: {
                int i = read_int("  Row i: "), j = read_int("  Col j: ");
                try { std::cout << "  [" << i << "][" << j << "] = " << m->get(i, j) << "\n"; }
                catch (const std::exception& e) { std::cout << "  [!] " << e.what() << "\n"; }
                break;
            }
            case 3: {
                int i = read_int("  Row i: "), j = read_int("  Col j: ");
                double v = read_double("  Value: ");
                try { m->set(i, j, v); std::cout << "  Done.\n"; }
                catch (const std::exception& e) { std::cout << "  [!] " << e.what() << "\n"; }
                break;
            }
            case 4:
                std::cout << "  norm = " << std::fixed << std::setprecision(8) << m->norm() << "\n";
                break;
            case 5: {
                show_all(seq);
                int other = read_valid_idx(seq);
                try {
                    IMatrix<T>* res = m->add(*seq.get(other));
                    seq.append(res);
                    std::cout << "  Result saved as [" << seq.get_size() - 1 << "].\n";
                } catch (const std::exception& e) { std::cout << "  [!] " << e.what() << "\n"; }
                break;
            }
            case 6: {
                double sc = read_double("  Scalar: ");
                try {
                    IMatrix<T>* res = m->mult_scalar(sc);
                    seq.append(res);
                    std::cout << "  Result saved as [" << seq.get_size() - 1 << "].\n";
                } catch (const std::exception& e) { std::cout << "  [!] " << e.what() << "\n"; }
                break;
            }
            default: std::cout << "  [!] Unknown command.\n";
        }
    }
}

// ============================================================
// Matrix menu
// ============================================================

static void run_matrix_menu(Seq& seq) {
    for (;;) {
        std::cout << "\n=== Matrices (" << seq.get_size() << " stored) ===\n"
                  << "  1. Show all\n"
                  << "  2. Create & save\n"
                  << "  3. Select & operate\n"
                  << "  4. Delete\n"
                  << "  0. Back\n";
        int choice = read_int("Choice: ");
        if (choice == 0) return;

        switch (choice) {
            case 1: show_all(seq); break;
            case 2: {
                IMatrix<T>* m = create_matrix();
                if (m) {
                    seq.append(m);
                    std::cout << "  Saved as [" << seq.get_size() - 1 << "].\n";
                }
                break;
            }
            case 3: {
                if (seq.get_size() == 0) { std::cout << "  No matrices.\n"; break; }
                show_all(seq);
                matrix_ops(seq, read_valid_idx(seq));
                break;
            }
            case 4: {
                if (seq.get_size() == 0) { std::cout << "  No matrices.\n"; break; }
                show_all(seq);
                int idx = read_valid_idx(seq);
                delete seq.get(idx);   // destroy matrix object
                seq.remove_at(idx);    // remove pointer from sequence
                std::cout << "  Deleted. Remaining: " << seq.get_size() << "\n";
                break;
            }
            default: std::cout << "  [!] Unknown command.\n";
        }
    }
}

// ============================================================
// SOE — work
// ============================================================

static void print_solution(const Vector<T>& x, int n) {
    for (int i = 0; i < n; i++)
        std::cout << "    x[" << i << "] = "
                  << std::fixed << std::setprecision(8) << x.get(i) << "\n";
}

static void run_soe_work() {
    int n = read_int("\n  Number of equations n: ");

    SystemOfEquations<T>* soe = nullptr;
    std::cout << "  Fill: 1. Manual  2. Random\n";
    if (read_int("  Choice: ") == 2) {
        unsigned seed = static_cast<unsigned>(read_int("  Seed: "));
        SystemOfEquations<T> tmp = SystemOfEquations<T>::random(n, seed);
        soe = new SystemOfEquations<T>(tmp);
    } else {
        SquareMatrix<T> A(n);
        Vector<T> b(n);
        std::cout << "  Enter matrix A:\n";
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                A.set(i, j, read_double("    A[" + std::to_string(i) + "][" + std::to_string(j) + "] = "));
        std::cout << "  Enter vector b:\n";
        for (int i = 0; i < n; i++)
            b.set(i, read_double("    b[" + std::to_string(i) + "] = "));
        soe = new SystemOfEquations<T>(A, b);
    }

    std::cout << "\n" << *soe << "\n";

    for (;;) {
        std::cout << "\n--- SOE (" << n << "x" << n << ") ---\n"
                  << "  1. Solve — Gauss\n"
                  << "  2. Solve — Gauss + pivot\n"
                  << "  3. Solve — PLU\n"
                  << "  4. Print system\n"
                  << "  0. Back\n";
        int op = read_int("  Choice: ");
        if (op == 0) break;
        if (op == 4) { std::cout << *soe; continue; }
        if (op < 1 || op > 3) { std::cout << "  [!] Unknown command.\n"; continue; }

        Vector<T>* x = nullptr;
        try {
            if (op == 1)      x = soe->solve_gauss();
            else if (op == 2) x = soe->solve_gauss_with_pivot();
            else              x = soe->solve_plu();

            std::cout << "  Solution:\n";
            print_solution(*x, n);
            std::cout << "  Residual ||Ax-b||/||b|| = "
                      << std::scientific << std::setprecision(4) << soe->residual(*x) << "\n";
            delete x;
        } catch (const std::exception& e) {
            std::cout << "  [!] " << e.what() << "\n";
            delete x;
        }
    }

    delete soe;
}

// ============================================================
// Experiments
// ============================================================

using Clock = std::chrono::high_resolution_clock;
using Ms    = std::chrono::duration<double, std::milli>;

// 4.1 — time for one system, methods compared
static void experiment_4_1() {
    const int     sizes[] = {100, 200, 500, 1000};
    const unsigned seed   = 42;

    std::cout << "\n--- Experiment 4.1: Time for one system (seed=" << seed << ") ---\n";
    std::cout << std::setw(6)  << "n"
              << std::setw(14) << "Gauss (ms)"
              << std::setw(18) << "Gauss+piv (ms)"
              << std::setw(18) << "PLU decomp (ms)"
              << std::setw(18) << "PLU subst (ms)"
              << "\n" << std::string(74, '-') << "\n";

    for (int n : sizes) {
        // Gauss
        SystemOfEquations<T> soe = SystemOfEquations<T>::random(n, seed);
        auto t0 = Clock::now();
        Vector<T>* x = soe.solve_gauss();
        double t_gauss = Ms(Clock::now() - t0).count();
        delete x;

        // Gauss + pivot (fresh soe — no shared mutable state, but good practice)
        soe = SystemOfEquations<T>::random(n, seed);
        t0 = Clock::now();
        x = soe.solve_gauss_with_pivot();
        double t_pivot = Ms(Clock::now() - t0).count();
        delete x;

        // PLU: first call = decompose + substitute
        SystemOfEquations<T> soe_plu = SystemOfEquations<T>::random(n, seed);
        t0 = Clock::now();
        x = soe_plu.solve_plu();   // lu_ready: false -> true
        double t_full = Ms(Clock::now() - t0).count();
        delete x;

        // PLU: second call = substitute only (lu_ready=true, same b)
        t0 = Clock::now();
        x = soe_plu.solve_plu();
        double t_subst = Ms(Clock::now() - t0).count();
        delete x;

        std::cout << std::setw(6)  << n
                  << std::setw(14) << std::fixed << std::setprecision(2) << t_gauss
                  << std::setw(18) << t_pivot
                  << std::setw(18) << (t_full - t_subst)
                  << std::setw(18) << t_subst
                  << "\n";
    }
}

// 4.2 — multiple rhs: Gauss+pivot k× vs PLU 1 decomp + k× subst
static void experiment_4_2() {
    const int     n      = 500;
    const int     ks[]   = {1, 10, 100};
    const unsigned seed  = 42;
    const int     max_k  = 100;

    std::cout << "\n--- Experiment 4.2: Multiple RHS (n=" << n << ", seed=" << seed << ") ---\n";
    std::cout << std::setw(6)  << "k"
              << std::setw(24) << "Gauss+piv k× (ms)"
              << std::setw(24) << "PLU 1xdecomp+kxsub (ms)"
              << "\n" << std::string(54, '-') << "\n";

    // pre-generate max_k rhs vectors
    std::mt19937 rng(seed + 1);
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    Vector<T>* rhs[max_k];
    for (int i = 0; i < max_k; i++) {
        rhs[i] = new Vector<T>(n);
        for (int j = 0; j < n; j++)
            rhs[i]->set(j, AlgebraTraits<T>::random_value(rng, dist));
    }

    for (int k : ks) {
        // Gauss+pivot: k full O(n³) solves
        SystemOfEquations<T> soe_g = SystemOfEquations<T>::random(n, seed);
        auto t0 = Clock::now();
        for (int i = 0; i < k; i++) {
            soe_g.set_rhs(*rhs[i]);
            Vector<T>* x = soe_g.solve_gauss_with_pivot();
            delete x;
        }
        double t_gauss = Ms(Clock::now() - t0).count();

        // PLU: 1×O(n³) decompose + k×O(n²) substitutions
        // set_rhs does NOT reset lu_ready — key pattern
        SystemOfEquations<T> soe_plu = SystemOfEquations<T>::random(n, seed);
        t0 = Clock::now();
        for (int i = 0; i < k; i++) {
            soe_plu.set_rhs(*rhs[i]);
            Vector<T>* x = soe_plu.solve_plu();
            delete x;
        }
        double t_plu = Ms(Clock::now() - t0).count();

        std::cout << std::setw(6)  << k
                  << std::setw(24) << std::fixed << std::setprecision(2) << t_gauss
                  << std::setw(24) << t_plu
                  << "\n";
    }

    for (int i = 0; i < max_k; i++) delete rhs[i];
}

// 4.3 — Hilbert matrix accuracy
static void experiment_4_3() {
    const int sizes[] = {5, 10, 15};

    std::cout << "\n--- Experiment 4.3: Hilbert accuracy (x_exact = [1,...,1]) ---\n";

    for (int n : sizes) {
        std::cout << "\n  n = " << n << ":\n";
        std::cout << "  " << std::setw(14) << "Method"
                  << std::setw(16) << "Residual"
                  << std::setw(16) << "Rel.Error"
                  << "\n  " << std::string(44, '-') << "\n";

        SystemOfEquations<T> soe = SystemOfEquations<T>::hilbert(n);

        Vector<T> exact(n);
        for (int i = 0; i < n; i++) exact.set(i, 1.0);

        const char* names[] = {"Gauss", "Gauss+pivot", "PLU"};
        for (int m = 0; m < 3; m++) {
            Vector<T>* x = nullptr;
            try {
                if (m == 0)      x = soe.solve_gauss();
                else if (m == 1) x = soe.solve_gauss_with_pivot();
                else             x = soe.solve_plu();

                std::cout << "  " << std::setw(14) << names[m]
                          << std::setw(16) << std::scientific << std::setprecision(4) << soe.residual(*x)
                          << std::setw(16) << SystemOfEquations<T>::relative_error(*x, exact)
                          << "\n";
                delete x;
            } catch (const std::exception& e) {
                std::cout << "  " << std::setw(14) << names[m] << "  [!] " << e.what() << "\n";
                delete x;
            }
        }
    }
    std::cout << "\n";
}

static void run_experiments() {
    std::cout << "\n=== Running all experiments (n=1000 may take ~30s) ===\n";
    try { experiment_4_1(); } catch (const std::exception& e) { std::cout << "[!] Exp 4.1: " << e.what() << "\n"; }
    try { experiment_4_2(); } catch (const std::exception& e) { std::cout << "[!] Exp 4.2: " << e.what() << "\n"; }
    try { experiment_4_3(); } catch (const std::exception& e) { std::cout << "[!] Exp 4.3: " << e.what() << "\n"; }
    std::cout << "=== Done ===\n";
}

// ============================================================
// SOE menu
// ============================================================

static void run_soe_menu() {
    for (;;) {
        std::cout << "\n=== System of Linear Equations ===\n"
                  << "  1. Work\n"
                  << "  2. Experiments\n"
                  << "  0. Back\n";
        int c = read_int("Choice: ");
        if (c == 0) return;
        if (c == 1) run_soe_work();
        else if (c == 2) run_experiments();
        else std::cout << "  [!] Unknown command.\n";
    }
}

// ============================================================
// main
// ============================================================

int main() {
    Seq matrices;

    for (;;) {
        std::cout << "\n==============================\n"
                  << "        MAIN MENU\n"
                  << "==============================\n"
                  << "  1. Matrices\n"
                  << "  2. System of Linear Equations\n"
                  << "  0. Exit\n";
        int c = read_int("Choice: ");
        if (c == 0) break;
        else if (c == 1) run_matrix_menu(matrices);
        else if (c == 2) run_soe_menu();
        else std::cout << "  [!] Unknown command.\n";
    }

    // cleanup — delete all remaining matrix objects
    for (int i = 0; i < matrices.get_size(); i++)
        delete matrices.get(i);

    return 0;
}