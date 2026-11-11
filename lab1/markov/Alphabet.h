/**
 * @file Alphabet.h
 * @brief Алфавит нормального алгорифма Маркова.
 */
#ifndef LAB_ALPHABET_H
#define LAB_ALPHABET_H

#include <cstddef>
#include <iosfwd>
#include <set>
#include <string>

namespace lab {

/**
 * @brief Множество допустимых символов.
 * @details Недопустимы пробельные символы, '.' (признак заключительной
 *          подстановки) и '#' (признак комментария в файле программы).
 */
class Alphabet {
public:
    /// @brief Пустой алфавит.
    Alphabet() = default;

    /**
     * @brief Алфавит из символов строки (повторы игнорируются).
     * @param symbols строка символов
     * @throw std::invalid_argument если есть недопустимый символ
     */
    explicit Alphabet(const std::string& symbols);

    /**
     * @brief Добавляет символ.
     * @param c символ
     * @throw std::invalid_argument если символ недопустим
     */
    void add(char c);

    /// @brief Удаляет символ. @return true, если символ был в алфавите
    bool remove(char c);

    /// @brief @return true, если символ принадлежит алфавиту
    bool contains(char c) const;

    /// @brief @return true, если все символы строки принадлежат алфавиту
    bool contains(const std::string& s) const;

    /// @brief @return число символов
    std::size_t size() const;

    /// @brief @return true, если алфавит пуст
    bool empty() const;

    /// @brief @return символы алфавита в порядке возрастания кодов
    std::string toString() const;

    /// @brief Допустим ли символ в принципе (не пробел, не '.', не '#').
    static bool isValidSymbol(char c);

private:
    std::set<char> symbols_; ///< символы алфавита
};

/// @brief Равенство алфавитов.
bool operator==(const Alphabet& a, const Alphabet& b);
/// @brief Неравенство алфавитов.
bool operator!=(const Alphabet& a, const Alphabet& b);

/// @brief Вывод символов алфавита подряд.
std::ostream& operator<<(std::ostream& out, const Alphabet& a);

/**
 * @brief Ввод алфавита: читается одно слово (до пробела).
 * @details При неудаче или недопустимом символе выставляется failbit, а
 *          объект не изменяется.
 */
std::istream& operator>>(std::istream& in, Alphabet& a);

} // namespace lab

#endif
