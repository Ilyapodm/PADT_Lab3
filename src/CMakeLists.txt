# ─── Библиотека с твоими структурами данных ───────────────────────────────────
add_library(seq_lib INTERFACE)
# INTERFACE — специальный тип для header-only библиотек (когда вся реализация
# в .hpp файлах, что типично для шаблонных классов).
# Если появятся .cpp файлы с реализацией — замени INTERFACE на STATIC:
#   add_library(seq_lib STATIC
#       DynamicArray.cpp    # <- сюда добавляй новые .cpp файлы
#       LinkedList.cpp
#   )

target_include_directories(seq_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Говорим CMake: "все, кто линкует seq_lib, автоматически получают
    # доступ к заголовкам из папки src/".
    # Благодаря этому в тестах и main.cpp можно писать просто:
    #   #include "DynamicArray.hpp"
    # а не полный путь типа "../src/DynamicArray.hpp"
)

# Если добавляешь новый .hpp файл — просто кладёшь его в src/, 
# в CMakeLists.txt ничего менять не нужно (INTERFACE сам найдёт через include dir).
# Если добавляешь новый .cpp файл — добавь его в список выше после замены на STATIC.

# отдельная для не шаблонного кода
add_library(bit_lib STATIC
    bit_sequence.cpp
)

target_include_directories(bit_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bit_lib PUBLIC seq_lib)

# ─── Исполняемый файл (интерактивная программа / UI) ──────────────────────────
add_executable(lab2
    main.cpp
    # Если разобьёшь main на несколько .cpp файлов — добавляй их сюда:
    # ui.cpp
    # menu.cpp
)

target_link_libraries(lab2 PRIVATE bit_lib)
# PRIVATE — seq_lib нужна только main_app, не "пробрасывается" дальше.
# Это стандартный выбор для исполняемых файлов.
