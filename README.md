# 2 Lab O_o

## Как запустить
1. Из корня проекта:
- cmake -B build                  # конфигурация (один раз / после изменений CMake)
- cmake --build build             # сборка

2. Запустить все тесты:
cd build && ctest --output-on-failure

3. Запустить только исполняемую программу:
./build/src/lab2

4. Запустить тесты напрямую с фильтрацией:
./build/tests/my_tests --gtest_filter="DynamicArray*"

## Детали реализации
### BitSequence
BitSequence - конкретная реализация Sequence (Sequence\<Bit\>) с дополнительными битовыми операциями
- BitSequence - всегда immutable (все методы возвращают новый объект)

### get_subsequence()
Результат get_subsequence() следует типу операнда (*this)
- immutable->get_subsequence() → Immutable
- mutable->get_subsequence()   → Mutable

### concat():
Результат concat следует типу левого операнда (*this)
- mutable->concat(immutable)   → Mutable    ("расширяем mutable")
- immutable->concat(mutable)   → Immutable  ("расширяем immutable")
- immutable->concat(immutable) → Immutable
- mutable->concat(mutable)     → Mutable

### IEnumerator<T>
Везде включен через nested classes.

Только const! Нет set_current()!

- IEnumerator<T> для ArraySequence реализован через сам ArraySequence (why not, все необходимое есть)
- IEnumerator<T> для ListSequence реализован через LinkedList (делегирование вниз). Так мы не тратим O(n) при использовании get

### Управление памятью в ArraySequence
Управление памятью для ArraySequence делегировано вниз: в DynamicArray. Именно здесь хранятся size (количество элементов) и capacity(уже выделенные ячейки памяти под этот массив). 
#### Метод resize() в DynamicArray
Работа с размером массива происходит всегда через этот метод.
resize() увеличивает size на заданное количество и не меняет capacity, если size + new_size <= capacity. Если же capacity не хватает для всех size + new_size - то выделяется 2x памяти от исходного capacity, и туда копируются данные.
PS. Если не хватит 2x capacity, то выделиться size + new_size ячеек.