/**
 * @file MarkovProgram.cpp
 * @brief Реализация lab::MarkovProgram.
 */
#include "MarkovProgram.h"
#include "StringUtils.h"

#include <istream>
#include <ostream>
#include <stdexcept>

namespace lab {

MarkovProgram::MarkovProgram(const Alphabet& alphabet) : alphabet_(alphabet) {}

const Alphabet& MarkovProgram::alphabet() const { return alphabet_; }

void MarkovProgram::validate(const MarkovRule& rule) const {
    if (!alphabet_.contains(rule.pattern()) || !alphabet_.contains(rule.replacement()))
        throw std::invalid_argument("MarkovProgram: rule uses symbols outside the alphabet");
}

void MarkovProgram::setAlphabet(const Alphabet& alphabet) {
    MarkovProgram candidate(alphabet);
    for (const MarkovRule& r : rules_) candidate.validate(r);
    alphabet_ = alphabet;
}

std::size_t MarkovProgram::size() const { return rules_.size(); }
bool MarkovProgram::empty() const { return rules_.empty(); }

void MarkovProgram::addRule(const MarkovRule& rule) {
    validate(rule);
    rules_.push_back(rule);
}

void MarkovProgram::insertRule(std::size_t pos, const MarkovRule& rule) {
    if (pos > rules_.size())
        throw std::out_of_range("MarkovProgram::insertRule: bad position");
    validate(rule);
    rules_.insert(rules_.begin() + static_cast<std::ptrdiff_t>(pos), rule);
}

void MarkovProgram::removeRule(std::size_t pos) {
    if (pos >= rules_.size())
        throw std::out_of_range("MarkovProgram::removeRule: bad index");
    rules_.erase(rules_.begin() + static_cast<std::ptrdiff_t>(pos));
}

void MarkovProgram::clear() { rules_.clear(); }

const MarkovRule& MarkovProgram::operator[](std::size_t pos) const {
    if (pos >= rules_.size())
        throw std::out_of_range("MarkovProgram::operator[]: bad index");
    return rules_[pos];
}

std::optional<std::size_t> MarkovProgram::findApplicable(const std::string& text) const {
    for (std::size_t i = 0; i < rules_.size(); ++i)
        if (rules_[i].matches(text)) return i;
    return std::nullopt;
}

void MarkovProgram::load(std::istream& in, std::string* tape) {
    Alphabet alphabet;
    std::string tapeText;
    std::vector<MarkovRule> rules;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        if (line.compare(0, 9, "alphabet:") == 0) {
            alphabet = Alphabet(trim(line.substr(9)));
        } else if (line.compare(0, 5, "tape:") == 0) {
            tapeText = trim(line.substr(5));
        } else {
            rules.push_back(MarkovRule::parse(line));
        }
    }
    MarkovProgram candidate(alphabet);
    for (const MarkovRule& r : rules) candidate.addRule(r);
    *this = candidate;
    if (tape) *tape = tapeText;
}

bool operator==(const MarkovProgram& a, const MarkovProgram& b) {
    if (a.alphabet() != b.alphabet() || a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) return false;
    return true;
}
bool operator!=(const MarkovProgram& a, const MarkovProgram& b) { return !(a == b); }

std::ostream& operator<<(std::ostream& out, const MarkovProgram& p) {
    out << "alphabet: " << p.alphabet() << '\n';
    for (std::size_t i = 0; i < p.size(); ++i) out << p[i] << '\n';
    return out;
}

std::istream& operator>>(std::istream& in, MarkovProgram& p) {
    try {
        p.load(in);
    } catch (const std::exception&) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

} // namespace lab
