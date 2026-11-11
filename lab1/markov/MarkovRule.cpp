/**
 * @file MarkovRule.cpp
 * @brief Реализация lab::MarkovRule.
 */
#include "MarkovRule.h"
#include "StringUtils.h"

#include <istream>
#include <ostream>
#include <stdexcept>

namespace lab {

MarkovRule::MarkovRule() : terminal_(false) {}

MarkovRule::MarkovRule(const std::string& pattern, const std::string& replacement,
                       bool terminal)
    : pattern_(pattern), replacement_(replacement), terminal_(terminal) {}

MarkovRule MarkovRule::parse(const std::string& text) {
    const std::string::size_type pos = text.find("->");
    if (pos == std::string::npos)
        throw std::runtime_error("MarkovRule::parse: missing '->' in \"" + text + "\"");
    std::string pattern = trim(text.substr(0, pos));
    std::string rest = trim(text.substr(pos + 2));
    bool terminal = false;
    if (!rest.empty() && rest[0] == '.') {
        terminal = true;
        rest = trim(rest.substr(1));
    }
    return MarkovRule(pattern, rest, terminal);
}

const std::string& MarkovRule::pattern() const { return pattern_; }
const std::string& MarkovRule::replacement() const { return replacement_; }
bool MarkovRule::isTerminal() const { return terminal_; }

bool MarkovRule::matches(const std::string& text) const {
    return text.find(pattern_) != std::string::npos;
}

bool MarkovRule::apply(std::string& text) const {
    const std::string::size_type pos = text.find(pattern_);
    if (pos == std::string::npos) return false;
    text.replace(pos, pattern_.size(), replacement_);
    return true;
}

bool operator==(const MarkovRule& a, const MarkovRule& b) {
    return a.pattern() == b.pattern() && a.replacement() == b.replacement() &&
           a.isTerminal() == b.isTerminal();
}
bool operator!=(const MarkovRule& a, const MarkovRule& b) { return !(a == b); }

std::ostream& operator<<(std::ostream& out, const MarkovRule& r) {
    out << r.pattern() << (r.isTerminal() ? " ->. " : " -> ") << r.replacement();
    return out;
}

std::istream& operator>>(std::istream& in, MarkovRule& r) {
    std::string line;
    if (!std::getline(in, line)) return in;
    try {
        r = MarkovRule::parse(line);
    } catch (const std::runtime_error&) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

} // namespace lab
