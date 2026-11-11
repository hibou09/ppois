/**
 * @file tests_rectangle.cpp
 * @brief Unit-тесты (UnitTest++) для lab::Rectangle.
 */
#include <UnitTest++/UnitTest++.h>
#include <sstream>
#include <stdexcept>
#include "Rectangle.h"

using lab::Rectangle;
using lab::Point;

TEST(Rect_DefaultIsZero) {
    Rectangle r;
    CHECK_EQUAL(0, r.left()); CHECK_EQUAL(0, r.bottom());
    CHECK_EQUAL(0, r.right()); CHECK_EQUAL(0, r.top());
}

TEST(Rect_ConstructorKeepsOrder) {
    Rectangle r(1, 2, 5, 7);
    CHECK_EQUAL(1, r.left()); CHECK_EQUAL(2, r.bottom());
    CHECK_EQUAL(5, r.right()); CHECK_EQUAL(7, r.top());
}

TEST(Rect_ConstructorNormalizesSwappedVertices) {
    Rectangle r(5, 7, 1, 2);
    CHECK_EQUAL(1, r.left()); CHECK_EQUAL(2, r.bottom());
    CHECK_EQUAL(5, r.right()); CHECK_EQUAL(7, r.top());
}

TEST(Rect_FromSize) {
    Rectangle r = Rectangle::fromSize(1, 1, 3, 4);
    CHECK(r == Rectangle(1, 1, 4, 5));
}

TEST(Rect_FromSizeNegativeThrows) {
    CHECK_THROW(Rectangle::fromSize(0, 0, -1, 2), std::invalid_argument);
}

TEST(Rect_Vertices) {
    Rectangle r(0, 0, 4, 3);
    CHECK(r.bottomLeft() == (Point{0, 0}));
    CHECK(r.bottomRight() == (Point{4, 0}));
    CHECK(r.topLeft() == (Point{0, 3}));
    CHECK(r.topRight() == (Point{4, 3}));
}

TEST(Rect_WidthHeightArea) {
    Rectangle r(-2, -1, 3, 4);
    CHECK_EQUAL(5, r.width());
    CHECK_EQUAL(5, r.height());
    CHECK_EQUAL(25, r.area());
}

TEST(Rect_MoveShiftsAllVertices) {
    Rectangle r(0, 0, 2, 3);
    r.move(1, -1);
    CHECK(r == Rectangle(1, -1, 3, 2));
}

TEST(Rect_MoveKeepsSize) {
    Rectangle r(0, 0, 2, 3);
    r.move(10, 10);
    CHECK_EQUAL(2, r.width()); CHECK_EQUAL(3, r.height());
}

TEST(Rect_MoveToSetsBottomLeft) {
    Rectangle r(5, 5, 7, 8);
    r.moveTo(0, 0);
    CHECK(r == Rectangle(0, 0, 2, 3));
}

TEST(Rect_ResizeKeepsBottomLeft) {
    Rectangle r(1, 1, 2, 2);
    r.resize(5, 6);
    CHECK(r == Rectangle(1, 1, 6, 7));
}

TEST(Rect_ResizeNegativeThrowsAndKeepsObject) {
    Rectangle r(1, 1, 2, 2);
    CHECK_THROW(r.resize(-1, 3), std::invalid_argument);
    CHECK(r == Rectangle(1, 1, 2, 2));
}

TEST(Rect_PreIncrement) {
    Rectangle r(0, 0, 1, 1);
    Rectangle& ref = ++r;
    CHECK(&ref == &r);
    CHECK(r == Rectangle(0, 0, 2, 2));
}

TEST(Rect_PostIncrementReturnsOldValue) {
    Rectangle r(0, 0, 1, 1);
    Rectangle old = r++;
    CHECK(old == Rectangle(0, 0, 1, 1));
    CHECK(r == Rectangle(0, 0, 2, 2));
}

TEST(Rect_PreDecrement) {
    Rectangle r(0, 0, 3, 3);
    --r;
    CHECK(r == Rectangle(0, 0, 2, 2));
}

TEST(Rect_PostDecrementReturnsOldValue) {
    Rectangle r(0, 0, 3, 3);
    Rectangle old = r--;
    CHECK(old == Rectangle(0, 0, 3, 3));
    CHECK(r == Rectangle(0, 0, 2, 2));
}

TEST(Rect_DecrementZeroSideThrowsAndKeepsObject) {
    Rectangle r(0, 0, 0, 5);
    CHECK_THROW(--r, std::domain_error);
    CHECK(r == Rectangle(0, 0, 0, 5));
}

TEST(Rect_UnionOperatorPlus) {
    Rectangle a(0, 0, 2, 2), b(1, 1, 5, 4);
    CHECK((a + b) == Rectangle(0, 0, 5, 4));
}

TEST(Rect_UnionDoesNotChangeOperands) {
    Rectangle a(0, 0, 2, 2), b(1, 1, 5, 4);
    Rectangle c = a + b;
    CHECK(a == Rectangle(0, 0, 2, 2));
    CHECK(b == Rectangle(1, 1, 5, 4));
    (void)c;
}

TEST(Rect_PlusAssignReturnsSelfReference) {
    Rectangle a(0, 0, 1, 1), b(3, 3, 4, 4);
    Rectangle& ref = (a += b);
    CHECK(&ref == &a);
    CHECK(a == Rectangle(0, 0, 4, 4));
}

TEST(Rect_IntersectionOperatorMinus) {
    Rectangle a(0, 0, 4, 4), b(2, 1, 6, 3);
    CHECK((a - b) == Rectangle(2, 1, 4, 3));
}

TEST(Rect_IntersectionOfDisjointIsZero) {
    Rectangle a(0, 0, 1, 1), b(5, 5, 6, 6);
    CHECK(!a.intersects(b));
    CHECK((a - b) == Rectangle());
}

TEST(Rect_IntersectionTouchingEdgeIsDegenerate) {
    Rectangle a(0, 0, 2, 2), b(2, 0, 4, 2);
    CHECK(a.intersects(b));
    CHECK((a - b) == Rectangle(2, 0, 2, 2));
}

TEST(Rect_MinusAssignReturnsSelfReference) {
    Rectangle a(0, 0, 4, 4), b(1, 1, 2, 2);
    Rectangle& ref = (a -= b);
    CHECK(&ref == &a);
    CHECK(a == Rectangle(1, 1, 2, 2));
}

TEST(Rect_EqualityAndInequality) {
    CHECK(Rectangle(0, 0, 1, 1) == Rectangle(1, 1, 0, 0));
    CHECK(Rectangle(0, 0, 1, 1) != Rectangle(0, 0, 1, 2));
}

TEST(Rect_CopyConstructorIsIndependent) {
    Rectangle a(0, 0, 1, 1);
    Rectangle b(a);
    b.move(5, 5);
    CHECK(a == Rectangle(0, 0, 1, 1));
}

TEST(Rect_AssignmentChainAndSelfAssignment) {
    Rectangle a(0, 0, 1, 1), b, c;
    c = b = a;
    CHECK(c == a);
    Rectangle& self = a;
    a = self;
    CHECK(a == Rectangle(0, 0, 1, 1));
}

TEST(Rect_StreamOutput) {
    std::ostringstream out;
    out << Rectangle(1, 2, 3, 4);
    CHECK_EQUAL("(1, 2)-(3, 4)", out.str());
}

TEST(Rect_StreamInput) {
    std::istringstream in("5 6 1 2");
    Rectangle r;
    in >> r;
    CHECK(r == Rectangle(1, 2, 5, 6));
}

TEST(Rect_StreamInputFailureSetsFailbitAndKeepsObject) {
    std::istringstream in("1 2 x 4");
    Rectangle r(7, 7, 8, 8);
    in >> r;
    CHECK(in.fail());
    CHECK(r == Rectangle(7, 7, 8, 8));
}

TEST(Rect_ConstObjectCanUseConstMethods) {
    const Rectangle r(0, 0, 2, 2);
    CHECK_EQUAL(4, r.area());
    CHECK_EQUAL(2, r.width());
}

int main() { return UnitTest::RunAllTests(); }
