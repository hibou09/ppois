/**
 * @file MarkovRule.h
 * @brief Правило (формула подстановки) нормального алгорифма Маркова.
 */
#ifndef LAB_MARKOV_RULE_H
#define LAB_MARKOV_RULE_H

#include <iosfwd>
#include <string>

namespace lab {

/**
 * @brief Формула подстановки «образец -> замена» (обычная) или
 *        «образец ->. замена» (заключительная).
 * @details Образец и замена могут быть пустыми строками. Пустой образец
 *          встречается в начале любой строки.
 */
class MarkovRule {
public:
    /// @brief Пустое обычное правило « -> ».
    MarkovRule();

    /**
     * @brief Создаёт правило.
     * @param pattern искомая подстрока
     * @param replacement строка замены
     * @param terminal true для заключительной подстановки
     */
    MarkovRule(const std::string& pattern, const std::string& replacement,
               bool terminal = false);

    /**
     * @brief Разбирает текст вида "a -> b" или "a ->. b".
     * @param text строка правила
     * @return правило
     * @throw std::runtime_error если нет разделителя "->"
     */
    static MarkovRule parse(const std::string& text);

    const std::string& pattern() const;     ///< @return образец
    const std::string& replacement() const; ///< @return замена
    bool isTerminal() const;                ///< @return true, если заключительное

    /// @brief @return true, если образец входит в строку
    bool matches(const std::string& text) const;

    /**
     * @brief Заменяет самое левое вхождение образца.
     * @param text изменяемая строка
     * @return true, если замена выполнена
     */
    bool apply(std::string& text) const;

private:
    std::string pattern_;     ///< образец
    std::string replacement_; ///< замена
    bool terminal_;           ///< признак заключительной подстановки
};

/// @brief Равенство правил.
bool operator==(const MarkovRule& a, const MarkovRule& b);
/// @brief Неравенство правил.
bool operator!=(const MarkovRule& a, const MarkovRule& b);

/// @brief Вывод в формате "pattern -> replacement" / "pattern ->. replacement".
std::ostream& operator<<(std::ostream& out, const MarkovRule& r);

/**
 * @brief Ввод правила: читается строка целиком (см. MarkovRule::parse).
 * @details При ошибке разбора выставляется failbit, объект не меняется.
 */
std::istream& operator>>(std::istream& in, MarkovRule& r);

} // namespace lab

#endif
