#pragma once

#include <ostream>

class SystemOfEquations {
public:

    // int get_number_variables() const {
    //     return variable_vector.size();
    // }

    // Методы решения СЛАУ
    const std::vector<double>& solve_gauss();             // Стандартный метод Гаусса
    const std::vector<double>& solve_gauss_with_pivot();  // Метод Гаусса с выбором ведущего элемента
    const std::vector<double>& solve_lu();                // LU разложение
private:
    // std::vector<double> main_matrix;    // Главная матрица коэффициентов
    // std::vector<char> variable_vector;  // Вектор столбец переменных
    // std::vector<double> free_vector;    // Вектор столбец свободных членов
};

// std::ostream& operator<<(std::ostream &os, const SystemOfEquations &sys) {
//     for (int i = 0; i < sys.get_number_variables(); i++) {

//     }
// }