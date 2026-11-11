/**
 * @file MarkovProgram.h
 * @brief Программа нормального алгорифма Маркова: алфавит и упорядоченный список правил.
 */
#ifndef LAB_MARKOV_PROGRAM_H
#define LAB_MARKOV_PROGRAM_H

#include <cstddef>
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

#include "Alphabet.h"
#include "MarkovRule.h"

namespace lab {

/**
 * @brief Алфавит и упорядоченный список правил.
 * @details Инвариант: образец и замена каждого правила состоят только из
 *          символов алфавита. Порядок правил определяет их приоритет.
 *
 *          Формат текстового представления (см. load() и operator<<):
 *          @code
 *          # комментарий
 *          alphabet: ab
 *          tape: baba
 *          ba -> ab
 *          a ->. b
 *          @endcode
 */
class MarkovProgram {
public:
    /// @brief Программа с пустым алфавитом и без правил.
    MarkovProgram() = default;

    /// @brief Программа без правил с заданным алфавитом.
    explicit MarkovProgram(const Alphabet& alphabet);

    const Alphabet& alphabet() const; ///< @return алфавит

    /**
     * @brief Заменяет алфавит.
     * @throw std::invalid_argument если существующие правила не подходят
     *        под новый алфавит (программа не меняется)
     */
    void setAlphabet(const Alphabet& alphabet);

    std::size_t size() const; ///< @return число правил
    bool empty() const;       ///< @return true, если правил нет

    /**
     * @brief Добавляет правило в конец списка.
     * @throw std::invalid_argument если в правиле есть символы вне алфавита
     */
    void addRule(const MarkovRule& rule);

    /**
     * @brief Вставляет правило перед позицией pos (pos == size() — в конец).
     * @throw std::out_of_range если pos > size()
     * @throw std::invalid_argument если в правиле есть символы вне алфавита
     */
    void insertRule(std::size_t pos, const MarkovRule& rule);

    /**
     * @brief Удаляет правило.
     * @throw std::out_of_range если индекс неверен
     */
    void removeRule(std::size_t pos);

    /// @brief Удаляет все правила (алфавит сохраняется).
    void clear();

    /**
     * @brief Просмотр правила.
     * @throw std::out_of_range если индекс неверен
     */
    const MarkovRule& operator[](std::size_t pos) const;

    /**
     * @brief Ищет первое по порядку правило, применимое к строке.
     * @param text строка (лента)
     * @return индекс правила или std::nullopt
     */
    std::optional<std::size_t> findApplicable(const std::string& text) const;

    /**
     * @brief Загружает программу из потока (формат см. в описании класса).
     * @details Старое содержимое заменяется только при успешной загрузке.
     * @param in поток ввода
     * @param tape если не nullptr, сюда записывается строка из "tape:"
     *        (если такой строки нет — пустая); иначе "tape:" игнорируется
     * @throw std::runtime_error при синтаксической ошибке
     * @throw std::invalid_argument если правила не соответствуют алфавиту
     */
    void load(std::istream& in, std::string* tape = nullptr);

private:
    /// @brief Проверка принадлежности правила алфавиту.
    void validate(const MarkovRule& rule) const;

    Alphabet alphabet_;            ///< алфавит
    std::vector<MarkovRule> rules_; ///< правила по приоритету
};

/// @brief Равенство программ (алфавит и правила).
bool operator==(const MarkovProgram& a, const MarkovProgram& b);
/// @brief Неравенство программ.
bool operator!=(const MarkovProgram& a, const MarkovProgram& b);

/// @brief Вывод программы (строка "alphabet:" и правила), пригоден для load().
std::ostream& operator<<(std::ostream& out, const MarkovProgram& p);

/// @brief Ввод программы, эквивалентно MarkovProgram::load. При ошибке — failbit.
std::istream& operator>>(std::istream& in, MarkovProgram& p);

} // namespace lab

#endif
