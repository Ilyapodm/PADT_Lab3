# Executale file with tests
add_executable(tests
    test_sequence.cpp
    test_array_sequence.cpp
    test_list_sequence.cpp
    test_bit_sequence.cpp
    test_dynamic_array.cpp
    test_linked_list.cpp
)

target_link_libraries(tests
    PRIVATE
    bit_lib # дает и seq_lib
    GTest::gtest_main # GTest + готовый main(), который запускает все тесты
    # Если хочешь писать main() сам — замени на GTest::gtest (без _main)
)

gtest_discover_tests(tests)
# ОБЯЗАТЕЛЬНАЯ строка. Автоматически находит все TEST() и TEST_F() в my_tests
# и регистрирует их в CTest. Без неё `ctest` покажет "No tests were found".
