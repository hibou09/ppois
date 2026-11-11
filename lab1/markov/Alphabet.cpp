/**
 * @file Alphabet.cpp
 * @brief Реализация lab::Alphabet.
 */
#include "Alphabet.h"

#include <cctype>
#include <istream>
#include <ostream>
#include <stdexcept>

namespace lab {

bool Alphabet::isValidSymbol(char c) {
    return !std::isspace(static_cast<unsigned char>(c)) && c != '.' && c != '#';
}

Alphabet::Alphabet(const std::string& symbols) {
    for (char c : symbols) add(c);
}

void Alphabet::add(char c) {
    if (!isValidSymbol(c))
        throw std::invalid_argument("Alphabet::add: invalid symbol");
    symbols_.insert(c);
}

bool Alphabet::remove(char c) { return symbols_.erase(c) > 0; }

bool Alphabet::contains(char c) const { return symbols_.count(c) > 0; }

bool Alphabet::contains(const std::string& s) const {
    for (char c : s)
        if (!contains(c)) return false;
    return true;
}

std::size_t Alphabet::size() const { return symbols_.size(); }
bool Alphabet::empty() const { return symbols_.empty(); }

std::string Alphabet::toString() const {
    return std::string(symbols_.begin(), symbols_.end());
}

bool operator==(const Alphabet& a, const Alphabet& b) {
    return a.toString() == b.toString();
}
bool operator!=(const Alphabet& a, const Alphabet& b) { return !(a == b); }

std::ostream& operator<<(std::ostream& out, const Alphabet& a) {
    return out << a.toString();
}

std::istream& operator>>(std::istream& in, Alphabet& a) {
    std::string word;
    if (!(in >> word)) return in;
    try {
        a = Alphabet(word);
    } catch (const std::invalid_argument&) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

} // namespace lab
