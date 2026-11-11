/**
 * @file tests_markov.cpp
 * @brief Unit-тесты (UnitTest++) для классов нормальных алгорифмов Маркова.
 */
#include <UnitTest++/UnitTest++.h>
#include <sstream>
#include <stdexcept>

#include "Alphabet.h"
#include "MarkovRule.h"
#include "MarkovProgram.h"
#include "MarkovMachine.h"

using namespace lab;

namespace {
MarkovProgram sortProgram() {
    MarkovProgram p(Alphabet("ab"));
    p.addRule(MarkovRule("ba", "ab"));
    return p;
}
}

// ---------- Alphabet ----------
TEST(Alphabet_ConstructorRemovesDuplicates) {
    Alphabet a("abca");
    CHECK_EQUAL(3u, a.size());
    CHECK_EQUAL("abc", a.toString());
}

TEST(Alphabet_ContainsChar) {
    Alphabet a("ab");
    CHECK(a.contains('a'));
    CHECK(!a.contains('z'));
}

TEST(Alphabet_ContainsString) {
    Alphabet a("ab");
    CHECK(a.contains("abba"));
    CHECK(a.contains(""));
    CHECK(!a.contains("abc"));
}

TEST(Alphabet_RejectsSpace) {
    CHECK_THROW(Alphabet("a b"), std::invalid_argument);
}

TEST(Alphabet_RejectsDotAndHash) {
    Alphabet a;
    CHECK_THROW(a.add('.'), std::invalid_argument);
    CHECK_THROW(a.add('#'), std::invalid_argument);
}

TEST(Alphabet_Remove) {
    Alphabet a("ab");
    CHECK(a.remove('a'));
    CHECK(!a.remove('a'));
    CHECK_EQUAL(1u, a.size());
}

TEST(Alphabet_EqualityIgnoresOrderOfInput) {
    CHECK(Alphabet("ab") == Alphabet("ba"));
    CHECK(Alphabet("ab") != Alphabet("abc"));
}

TEST(Alphabet_StreamRoundTrip) {
    std::ostringstream out;
    out << Alphabet("cab");
    CHECK_EQUAL("abc", out.str());
    std::istringstream in("xyz");
    Alphabet a;
    in >> a;
    CHECK(a == Alphabet("xyz"));
}

// ---------- MarkovRule ----------
TEST(Rule_ParseSimple) {
    MarkovRule r = MarkovRule::parse("ab -> c");
    CHECK_EQUAL("ab", r.pattern());
    CHECK_EQUAL("c", r.replacement());
    CHECK(!r.isTerminal());
}

TEST(Rule_ParseTerminal) {
    MarkovRule r = MarkovRule::parse("a ->. b");
    CHECK(r.isTerminal());
    CHECK_EQUAL("b", r.replacement());
}

TEST(Rule_ParseEmptySides) {
    MarkovRule r1 = MarkovRule::parse("+ ->");
    CHECK_EQUAL("", r1.replacement());
    MarkovRule r2 = MarkovRule::parse("-> a");
    CHECK_EQUAL("", r2.pattern());
}

TEST(Rule_ParseWithoutArrowThrows) {
    CHECK_THROW(MarkovRule::parse("abc"), std::runtime_error);
}

TEST(Rule_ApplyReplacesLeftmostOccurrenceOnly) {
    MarkovRule r("a", "x");
    std::string s = "baab";
    CHECK(r.apply(s));
    CHECK_EQUAL("bxab", s);
}

TEST(Rule_ApplyNotMatching) {
    MarkovRule r("z", "x");
    std::string s = "abc";
    CHECK(!r.apply(s));
    CHECK_EQUAL("abc", s);
}

TEST(Rule_EmptyPatternPrepends) {
    MarkovRule r("", "S");
    std::string s = "ab";
    CHECK(r.apply(s));
    CHECK_EQUAL("Sab", s);
}

TEST(Rule_EqualityAndOutput) {
    CHECK(MarkovRule("a", "b") == MarkovRule("a", "b"));
    CHECK(MarkovRule("a", "b") != MarkovRule("a", "b", true));
    std::ostringstream out;
    out << MarkovRule("a", "b", true);
    CHECK_EQUAL("a ->. b", out.str());
}

TEST(Rule_StreamInputBadSetsFailbit) {
    std::istringstream in("no arrow here");
    MarkovRule r("q", "w");
    in >> r;
    CHECK(in.fail());
    CHECK(r == MarkovRule("q", "w"));
}

// ---------- MarkovProgram ----------
TEST(Program_AddRuleValidatesAlphabet) {
    MarkovProgram p(Alphabet("ab"));
    p.addRule(MarkovRule("a", "b"));
    CHECK_THROW(p.addRule(MarkovRule("a", "c")), std::invalid_argument);
    CHECK_EQUAL(1u, p.size());
}

TEST(Program_InsertRuleKeepsOrder) {
    MarkovProgram p(Alphabet("abc"));
    p.addRule(MarkovRule("a", "b"));
    p.addRule(MarkovRule("c", "a"));
    p.insertRule(1, MarkovRule("b", "c"));
    CHECK(p[1] == MarkovRule("b", "c"));
    CHECK_EQUAL(3u, p.size());
}

TEST(Program_RemoveRule) {
    MarkovProgram p = sortProgram();
    p.removeRule(0);
    CHECK(p.empty());
}

TEST(Program_IndexOutOfRangeThrows) {
    MarkovProgram p = sortProgram();
    CHECK_THROW(p[5], std::out_of_range);
    CHECK_THROW(p.removeRule(1), std::out_of_range);
    CHECK_THROW(p.insertRule(3, MarkovRule("a", "b")), std::out_of_range);
}

TEST(Program_FindApplicableRespectsPriority) {
    MarkovProgram p(Alphabet("ab"));
    p.addRule(MarkovRule("b", "a"));
    p.addRule(MarkovRule("a", "b"));
    auto idx = p.findApplicable("ab");
    CHECK(idx.has_value());
    CHECK_EQUAL(0u, *idx);
    CHECK(!p.findApplicable("").has_value());
}

TEST(Program_SetAlphabetFailsIfRulesBecomeInvalid) {
    MarkovProgram p = sortProgram();
    CHECK_THROW(p.setAlphabet(Alphabet("a")), std::invalid_argument);
    CHECK(p.alphabet() == Alphabet("ab"));
}

TEST(Program_LoadFromStreamWithTapeAndComments) {
    std::istringstream in("# comment\nalphabet: ab\ntape: ba\n\nba -> ab\na ->. b\n");
    MarkovProgram p;
    std::string tape;
    p.load(in, &tape);
    CHECK_EQUAL("ba", tape);
    CHECK_EQUAL(2u, p.size());
    CHECK(p[1].isTerminal());
}

TEST(Program_LoadBadRuleThrowsAndKeepsProgram) {
    MarkovProgram p = sortProgram();
    std::istringstream in("alphabet: ab\nbroken line\n");
    CHECK_THROW(p.load(in), std::runtime_error);
    CHECK_EQUAL(1u, p.size());
}

TEST(Program_OutputCanBeLoadedBack) {
    MarkovProgram p = sortProgram();
    std::stringstream ss;
    ss << p;
    MarkovProgram q;
    ss >> q;
    CHECK(p == q);
}

// ---------- MarkovMachine ----------
TEST(Machine_StepAppliesFirstApplicableRule) {
    MarkovMachine m(sortProgram(), "babab");
    CHECK(m.step());
    CHECK_EQUAL("abbab", m.tape());
    CHECK_EQUAL(1u, m.stepsDone());
    CHECK(m.lastRule().has_value());
}

TEST(Machine_RunSorts) {
    MarkovMachine m(sortProgram(), "babab");
    CHECK_EQUAL(3u, m.run());
    CHECK_EQUAL("aabbb", m.tape());
    CHECK(m.isHalted());
}

TEST(Machine_HaltsWhenNoRuleApplies) {
    MarkovMachine m(sortProgram(), "aabb");
    CHECK(!m.step());
    CHECK(m.isHalted());
    CHECK_EQUAL(0u, m.stepsDone());
}

TEST(Machine_TerminalRuleHaltsImmediately) {
    MarkovProgram p(Alphabet("abc"));
    p.addRule(MarkovRule("a", "c", true));
    MarkovMachine m(p, "baab");
    CHECK(m.step());
    CHECK(m.isHalted());
    CHECK_EQUAL("bcab", m.tape());
    CHECK(!m.step());
}

TEST(Machine_UnaryAddition) {
    MarkovProgram p(Alphabet("1+"));
    p.addRule(MarkovRule("+", ""));
    MarkovMachine m(p, "111+11");
    m.run();
    CHECK_EQUAL("11111", m.tape());
}

TEST(Machine_RunRespectsStepLimitOnLoop) {
    MarkovProgram p(Alphabet("a"));
    p.addRule(MarkovRule("a", "a"));
    MarkovMachine m(p, "a");
    CHECK_EQUAL(10u, m.run(10));
    CHECK(!m.isHalted());
}

TEST(Machine_PrefixIncrementMakesStep) {
    MarkovMachine m(sortProgram(), "ba");
    ++m;
    CHECK_EQUAL("ab", m.tape());
}

TEST(Machine_PostfixIncrementReturnsOldState) {
    MarkovMachine m(sortProgram(), "ba");
    MarkovMachine old = m++;
    CHECK_EQUAL("ba", old.tape());
    CHECK_EQUAL("ab", m.tape());
}

TEST(Machine_SetTapeValidatesAndResets) {
    MarkovMachine m(sortProgram(), "ba");
    m.run();
    m.setTape("ab");
    CHECK(!m.isHalted());
    CHECK_EQUAL(0u, m.stepsDone());
    CHECK_THROW(m.setTape("abc"), std::invalid_argument);
    CHECK_EQUAL("ab", m.tape());
}

TEST(Machine_SetSymbolAndIndexing) {
    MarkovMachine m(sortProgram(), "aa");
    m.setSymbol(1, 'b');
    CHECK_EQUAL('b', m[1]);
    CHECK_THROW(m.setSymbol(2, 'a'), std::out_of_range);
    CHECK_THROW(m.setSymbol(0, 'z'), std::invalid_argument);
    CHECK_THROW(m[5], std::out_of_range);
}

TEST(Machine_AddAndRemoveRules) {
    MarkovMachine m(sortProgram(), "ba");
    m.addRule(MarkovRule("a", "b"));
    CHECK_EQUAL(2u, m.program().size());
    m.removeRule(0);
    CHECK_EQUAL(1u, m.program().size());
}

TEST(Machine_LoadFromStream) {
    std::istringstream in("alphabet: ab\ntape: babab\nba -> ab\n");
    MarkovMachine m;
    in >> m;
    m.run();
    CHECK_EQUAL("aabbb", m.tape());
}

TEST(Machine_LoadErrorSetsFailbit) {
    std::istringstream in("alphabet: ab\ntape: abc\n");
    MarkovMachine m;
    in >> m;
    CHECK(in.fail());
}

TEST(Machine_BinaryIncrementProgram) {
    std::istringstream in(
        "alphabet: 01*+\ntape: 1011\n1* -> *0\n0* ->. 1\n* ->. 1\n"
        "+0 -> 0+\n+1 -> 1+\n+ -> *\n-> +\n");
    MarkovMachine m;
    in >> m;
    m.run();
    CHECK_EQUAL("1100", m.tape());
}

TEST(Machine_EqualityAndOutput) {
    MarkovMachine a(sortProgram(), "ba"), b(sortProgram(), "ba");
    CHECK(a == b);
    ++b;
    CHECK(a != b);
    std::ostringstream out;
    out << a;
    CHECK_EQUAL("alphabet: ab\nba -> ab\ntape: ba\n", out.str());
}

TEST(Machine_DescribeFormat) {
    MarkovMachine m(sortProgram(), "ba");
    ++m;
    CHECK_EQUAL("step 1: ab", m.describe());
}

int main() { return UnitTest::RunAllTests(); }
