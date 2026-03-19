cmake_minimum_required(VERSION 3.15)
# 3.14+ обязателен для FetchContent_MakeAvailable

project(Lab2 CXX)
# Lab2 — имя проекта, используется в IDE и в сообщениях CMake

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Если у тебя нет C++17 фич — можно поставить 14, но 17 рекомендуется

if(MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        CACHE STRING "" FORCE
    )
endif()

# ─── Подключаем папки с кодом ────────────────────────────────────────────────
# ПОРЯДОК ВАЖЕН: src должен быть раньше tests,
# потому что tests ссылается на таргет seq_lib из src
add_subdirectory(src)

# ─── Скачиваем и подключаем GoogleTest ───────────────────────────────────────
include(FetchContent)
# FetchContent — встроенный модуль CMake для подтягивания зависимостей из сети

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    # Фиксируй конкретный тег, а не main/master —
    # иначе сборка сломается при обновлении GTest без твоего ведома
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Нужно только для Windows (MSVC), на Mac/Linux — безвредно.
# Без этой строки на Windows Debug-сборка может упасть с ошибкой линковщика.

FetchContent_MakeAvailable(googletest)
# Скачивает GTest (только при первой сборке, потом кешируется),
# конфигурирует и делает его таргеты доступными: GTest::gtest, GTest::gtest_main

# ─── Включаем систему тестирования CMake ─────────────────────────────────────
enable_testing()
# Активирует команду ctest. БЕЗ ЭТОЙ СТРОКИ ctest не увидит ни одного теста.

include(GoogleTest)
# Подключает хелпер-модуль, который даёт команду gtest_discover_tests()

add_subdirectory(tests)