/**
 * @file main.cpp
 * @brief Консольная программа-интерпретатор нормальных алгорифмов Маркова.
 * @details Использование: markov <файл> [-log]
 *          Файл содержит алфавит, начальную ленту и правила (см. MarkovProgram).
 *          С ключом -log после каждого шага выводится состояние машины.
 */
#include <cstring>
#include <fstream>
#include <iostream>

#include "MarkovMachine.h"

int main(int argc, char* argv[]) {
    const char* path = nullptr;
    bool log = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-log") == 0) log = true;
        else path = argv[i];
    }
    if (!path) {
        std::cerr << "Usage: " << argv[0] << " <file> [-log]\n";
        return 2;
    }
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Cannot open file: " << path << '\n';
        return 2;
    }
    try {
        lab::MarkovMachine machine;
        machine.load(file);
        if (log) std::cout << machine.describe() << '\n';
        const std::size_t limit = 1000000;
        while (machine.stepsDone() < limit && machine.step()) {
            if (log) {
                std::cout << machine.describe() << "   [rule "
                          << *machine.lastRule() + 1 << ": "
                          << machine.program()[*machine.lastRule()] << "]\n";
            }
        }
        if (!machine.isHalted()) {
            std::cerr << "Step limit exceeded (possible infinite loop)\n";
            return 1;
        }
        std::cout << "Result: " << machine.tape() << " (" << machine.stepsDone()
                  << " steps)\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
