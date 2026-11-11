/**
 * @file MarkovMachine.h
 * @brief Машина, интерпретирующая нормальный алгорифм Маркова.
 */
#ifndef LAB_MARKOV_MACHINE_H
#define LAB_MARKOV_MACHINE_H

#include <cstddef>
#include <iosfwd>
#include <optional>
#include <string>

#include "MarkovProgram.h"

namespace lab {

/**
 * @brief Машина = программа + лента (строка) + состояние выполнения.
 * @details Шаг алгорифма: берётся первое по порядку применимое правило,
 *          в строке заменяется самое левое вхождение образца. Работа
 *          заканчивается, если применена заключительная подстановка или
 *          ни одно правило не применимо.
 *
 *          Класс не взаимодействует с консолью: вывод состояния идёт в
 *          потоки, а построчный лог формирует вызывающий код.
 */
class MarkovMachine {
public:
    /// @brief Машина с пустой программой и пустой лентой.
    MarkovMachine() = default;

    /**
     * @brief Машина с программой и начальной лентой.
     * @throw std::invalid_argument если лента содержит символы вне алфавита
     */
    explicit MarkovMachine(const MarkovProgram& program, const std::string& tape = "");

    const MarkovProgram& program() const; ///< @return программа

    /// @brief Заменяет программу, сохраняя ленту; сбрасывает выполнение.
    /// @throw std::invalid_argument если лента не соответствует новому алфавиту
    void setProgram(const MarkovProgram& program);

    /// @brief Добавляет правило в конец программы. @see MarkovProgram::addRule
    void addRule(const MarkovRule& rule);
    /// @brief Удаляет правило по индексу. @see MarkovProgram::removeRule
    void removeRule(std::size_t index);

    const std::string& tape() const; ///< @return текущее содержимое ленты

    /**
     * @brief Задаёт новую ленту и сбрасывает выполнение.
     * @throw std::invalid_argument если есть символы вне алфавита
     */
    void setTape(const std::string& tape);

    /**
     * @brief Читает символ ленты.
     * @throw std::out_of_range если позиция вне ленты
     */
    char operator[](std::size_t pos) const;

    /**
     * @brief Изменяет символ ленты.
     * @throw std::out_of_range если позиция вне ленты
     * @throw std::invalid_argument если символа нет в алфавите
     */
    void setSymbol(std::size_t pos, char c);

    bool isHalted() const;          ///< @return true, если машина остановилась
    std::size_t stepsDone() const;  ///< @return число выполненных шагов

    /// @brief Индекс правила, применённого на последнем шаге.
    std::optional<std::size_t> lastRule() const;

    /**
     * @brief Выполняет один шаг.
     * @return true, если правило применено; false, если машина остановлена
     *         (остановка фиксируется и на «пустом» шаге)
     */
    bool step();

    /**
     * @brief Выполняет программу до остановки или до лимита шагов.
     * @param maxSteps защита от зацикливания
     * @return число выполненных в этом вызове шагов
     */
    std::size_t run(std::size_t maxSteps = 1000000);

    /// @brief Префиксный ++ — один шаг. @return ссылка на себя
    MarkovMachine& operator++();
    /// @brief Постфиксный ++ — один шаг. @return копия до шага
    MarkovMachine operator++(int);

    /**
     * @brief Загружает программу и ленту из потока.
     * @see MarkovProgram::load
     */
    void load(std::istream& in);

    /// @brief @return строка вида "step N: tape" для лога
    std::string describe() const;

private:
    MarkovProgram program_;               ///< программа
    std::string tape_;                    ///< лента
    bool halted_ = false;                 ///< остановлена ли машина
    std::size_t steps_ = 0;               ///< число шагов
    std::optional<std::size_t> lastRule_; ///< последнее применённое правило
};

/// @brief Равенство: программа, лента и признак остановки.
bool operator==(const MarkovMachine& a, const MarkovMachine& b);
/// @brief Неравенство.
bool operator!=(const MarkovMachine& a, const MarkovMachine& b);

/// @brief Вывод программы и строки "tape:" в формате, пригодном для load().
std::ostream& operator<<(std::ostream& out, const MarkovMachine& m);

/// @brief Ввод программы и ленты. При ошибке — failbit.
std::istream& operator>>(std::istream& in, MarkovMachine& m);

} // namespace lab

#endif
