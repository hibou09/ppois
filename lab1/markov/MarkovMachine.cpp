/**
 * @file MarkovMachine.cpp
 * @brief Реализация lab::MarkovMachine.
 */
#include "MarkovMachine.h"

#include <istream>
#include <ostream>
#include <stdexcept>

namespace lab {

MarkovMachine::MarkovMachine(const MarkovProgram& program, const std::string& tape)
    : program_(program) {
    setTape(tape);
}

const MarkovProgram& MarkovMachine::program() const { return program_; }

void MarkovMachine::setProgram(const MarkovProgram& program) {
    if (!program.alphabet().contains(tape_))
        throw std::invalid_argument("MarkovMachine::setProgram: tape is outside the alphabet");
    program_ = program;
    halted_ = false; steps_ = 0; lastRule_.reset();
}

void MarkovMachine::addRule(const MarkovRule& rule) {
    program_.addRule(rule);
    halted_ = false;
}

void MarkovMachine::removeRule(std::size_t index) {
    program_.removeRule(index);
    halted_ = false;
}

const std::string& MarkovMachine::tape() const { return tape_; }

void MarkovMachine::setTape(const std::string& tape) {
    if (!program_.alphabet().contains(tape))
        throw std::invalid_argument("MarkovMachine::setTape: symbol outside the alphabet");
    tape_ = tape;
    halted_ = false; steps_ = 0; lastRule_.reset();
}

char MarkovMachine::operator[](std::size_t pos) const {
    if (pos >= tape_.size()) throw std::out_of_range("MarkovMachine::operator[]");
    return tape_[pos];
}

void MarkovMachine::setSymbol(std::size_t pos, char c) {
    if (pos >= tape_.size()) throw std::out_of_range("MarkovMachine::setSymbol");
    if (!program_.alphabet().contains(c))
        throw std::invalid_argument("MarkovMachine::setSymbol: symbol outside the alphabet");
    tape_[pos] = c;
    halted_ = false;
}

bool MarkovMachine::isHalted() const { return halted_; }
std::size_t MarkovMachine::stepsDone() const { return steps_; }
std::optional<std::size_t> MarkovMachine::lastRule() const { return lastRule_; }

bool MarkovMachine::step() {
    if (halted_) return false;
    const std::optional<std::size_t> idx = program_.findApplicable(tape_);
    if (!idx) {
        halted_ = true;
        return false;
    }
    const MarkovRule& rule = program_[*idx];
    rule.apply(tape_);
    lastRule_ = idx;
    ++steps_;
    if (rule.isTerminal()) halted_ = true;
    return true;
}

std::size_t MarkovMachine::run(std::size_t maxSteps) {
    std::size_t done = 0;
    while (done < maxSteps && step()) ++done;
    return done;
}

MarkovMachine& MarkovMachine::operator++() {
    step();
    return *this;
}

MarkovMachine MarkovMachine::operator++(int) {
    MarkovMachine old(*this);
    ++(*this);
    return old;
}

void MarkovMachine::load(std::istream& in) {
    MarkovProgram p;
    std::string tape;
    p.load(in, &tape);
    MarkovMachine fresh(p, tape);
    *this = fresh;
}

std::string MarkovMachine::describe() const {
    return "step " + std::to_string(steps_) + ": " + tape_;
}

bool operator==(const MarkovMachine& a, const MarkovMachine& b) {
    return a.program() == b.program() && a.tape() == b.tape() &&
           a.isHalted() == b.isHalted();
}
bool operator!=(const MarkovMachine& a, const MarkovMachine& b) { return !(a == b); }

std::ostream& operator<<(std::ostream& out, const MarkovMachine& m) {
    return out << m.program() << "tape: " << m.tape() << '\n';
}

std::istream& operator>>(std::istream& in, MarkovMachine& m) {
    try {
        m.load(in);
    } catch (const std::exception&) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

} // namespace lab
