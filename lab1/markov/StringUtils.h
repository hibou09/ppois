/**
 * @file StringUtils.h
 * @brief Мелкие вспомогательные функции работы со строками.
 */
#ifndef LAB_STRING_UTILS_H
#define LAB_STRING_UTILS_H

#include <string>

namespace lab {

/**
 * @brief Удаляет пробельные символы в начале и конце строки.
 * @param s исходная строка
 * @return строка без внешних пробелов
 */
inline std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    const std::string::size_type b = s.find_first_not_of(ws);
    if (b == std::string::npos) return std::string();
    const std::string::size_type e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

} // namespace lab

#endif
