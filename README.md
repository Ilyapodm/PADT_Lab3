# Polymorphic abstract data types lab №3

## Как запустить
1. Из корня проекта:
- cmake -B build                  # конфигурация (один раз / после изменений CMake)
- cmake --build build             # сборка

2. Запустить все тесты:
cd build && ctest --output-on-failure

3. Запустить только исполняемую программу:
./build/src/lab3

4. Запустить тесты напрямую с фильтрацией:
./build/tests/my_tests --gtest_filter="DynamicArray*"

## Архитектура
### 

### Метод resize() в DynamicArray
Работа с размером массива происходит всегда через этот метод.
resize() увеличивает size на заданное количество и не меняет capacity, если size + new_size <= capacity. Если же capacity не хватает для всех size + new_size - то выделяется 2x памяти от исходного capacity, и туда копируются данные.
PS. Если не хватит 2x capacity, то выделиться size + new_size ячеек.

## Реализованные бонусы
- .
- .