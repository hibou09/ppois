# Лабораторная работа №1 — Прямоугольник и нормальные алгорифмы Маркова

## Сборка
    sudo apt install libunittest++-dev cmake g++ doxygen
    cmake -B build && cmake --build build
    ./build/rectangle_tests        # 31 тест
    ./build/markov_tests           # 42 теста
    ./build/markov markov/examples/sort.txt -log

Без CMake: `g++ -std=c++17 -Irectangle rectangle/*.cpp -lUnitTest++ -o rt`

## Документация
    doxygen Doxyfile      # результат: docs/html/index.html

## Формат файла Маркова
    # комментарий
    alphabet: ab        # допустимые символы (без пробелов, '.', '#')
    tape: babab         # начальная лента
    ba -> ab            # обычная подстановка
    a ->. b             # заключительная подстановка
Правила применяются по порядку; пустая левая/правая часть допустима
(`-> a` добавляет `a` в начало, `+ ->` удаляет `+`).

## Структура
- `rectangle/` — класс `lab::Rectangle` (+ `Point`), тесты.
- `markov/` — `Alphabet`, `MarkovRule`, `MarkovProgram`, `MarkovMachine`, `main.cpp`, `examples/`, тесты.

## Принятые решения (можно изменить под требования преподавателя)
- Прямоугольник хранит левую нижнюю и правую верхнюю вершины; `++`/`--` сдвигают правую верхнюю.
- Пустое пересечение возвращает вырожденный прямоугольник (0,0)-(0,0); проверка — `intersects()`.
- `--` при нулевой стороне бросает `std::domain_error`.
- Деструкторы не нужны (правило нуля), копирование — `= default`.
