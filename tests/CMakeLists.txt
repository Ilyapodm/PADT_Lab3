# Executale file with tests
add_executable(tests
    test_dynamic_array.cpp
    test_diagonal_matrix.cpp
    test_matrix.cpp
    #test_sparse_matrix.cpp
    test_square_matrix.cpp
    test_system_of_equations.cpp
    test_triangular_matrix.cpp
    test_vector.cpp
)

target_link_libraries(tests
    PRIVATE
    matrix_lib 
    GTest::gtest_main # GTest + готовый main(), который запускает все тесты
)

gtest_discover_tests(tests)
# ОБЯЗАТЕЛЬНАЯ строка. Автоматически находит все TEST() и TEST_F() в my_tests
# и регистрирует их в CTest. Без неё `ctest` покажет "No tests were found".
