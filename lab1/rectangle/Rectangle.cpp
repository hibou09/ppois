/**
 * @file Rectangle.cpp
 * @brief Реализация класса lab::Rectangle.
 */
#include "Rectangle.h"

#include <algorithm>
#include <istream>
#include <ostream>
#include <stdexcept>

namespace lab {

bool operator==(const Point& a, const Point& b) { return a.x == b.x && a.y == b.y; }
bool operator!=(const Point& a, const Point& b) { return !(a == b); }

Rectangle::Rectangle() : left_(0), bottom_(0), right_(0), top_(0) {}

Rectangle::Rectangle(int x1, int y1, int x2, int y2)
    : left_(std::min(x1, x2)), bottom_(std::min(y1, y2)),
      right_(std::max(x1, x2)), top_(std::max(y1, y2)) {}

Rectangle Rectangle::fromSize(int x, int y, int width, int height) {
    if (width < 0 || height < 0)
        throw std::invalid_argument("Rectangle::fromSize: negative size");
    return Rectangle(x, y, x + width, y + height);
}

int Rectangle::left() const { return left_; }
int Rectangle::right() const { return right_; }
int Rectangle::bottom() const { return bottom_; }
int Rectangle::top() const { return top_; }
Point Rectangle::bottomLeft() const { return Point{left_, bottom_}; }
Point Rectangle::bottomRight() const { return Point{right_, bottom_}; }
Point Rectangle::topLeft() const { return Point{left_, top_}; }
Point Rectangle::topRight() const { return Point{right_, top_}; }
int Rectangle::width() const { return right_ - left_; }
int Rectangle::height() const { return top_ - bottom_; }
long long Rectangle::area() const {
    return static_cast<long long>(width()) * height();
}

Rectangle& Rectangle::move(int dx, int dy) {
    left_ += dx; right_ += dx;
    bottom_ += dy; top_ += dy;
    return *this;
}

Rectangle& Rectangle::moveTo(int x, int y) {
    return move(x - left_, y - bottom_);
}

Rectangle& Rectangle::resize(int newWidth, int newHeight) {
    if (newWidth < 0 || newHeight < 0)
        throw std::invalid_argument("Rectangle::resize: negative size");
    right_ = left_ + newWidth;
    top_ = bottom_ + newHeight;
    return *this;
}

Rectangle& Rectangle::operator++() {
    ++right_;
    ++top_;
    return *this;
}

Rectangle Rectangle::operator++(int) {
    Rectangle old(*this);
    ++(*this);
    return old;
}

Rectangle& Rectangle::operator--() {
    if (width() == 0 || height() == 0)
        throw std::domain_error("Rectangle::operator--: side is already zero");
    --right_;
    --top_;
    return *this;
}

Rectangle Rectangle::operator--(int) {
    Rectangle old(*this);
    --(*this);
    return old;
}

Rectangle& Rectangle::operator+=(const Rectangle& other) {
    left_ = std::min(left_, other.left_);
    bottom_ = std::min(bottom_, other.bottom_);
    right_ = std::max(right_, other.right_);
    top_ = std::max(top_, other.top_);
    return *this;
}

bool Rectangle::intersects(const Rectangle& other) const {
    return std::max(left_, other.left_) <= std::min(right_, other.right_) &&
           std::max(bottom_, other.bottom_) <= std::min(top_, other.top_);
}

Rectangle& Rectangle::operator-=(const Rectangle& other) {
    if (!intersects(other)) {
        left_ = bottom_ = right_ = top_ = 0;
        return *this;
    }
    const int l = std::max(left_, other.left_);
    const int b = std::max(bottom_, other.bottom_);
    const int r = std::min(right_, other.right_);
    const int t = std::min(top_, other.top_);
    left_ = l; bottom_ = b; right_ = r; top_ = t;
    return *this;
}

Rectangle operator+(Rectangle lhs, const Rectangle& rhs) {
    lhs += rhs;
    return lhs;
}

Rectangle operator-(Rectangle lhs, const Rectangle& rhs) {
    lhs -= rhs;
    return lhs;
}

bool operator==(const Rectangle& a, const Rectangle& b) {
    return a.bottomLeft() == b.bottomLeft() && a.topRight() == b.topRight();
}

bool operator!=(const Rectangle& a, const Rectangle& b) { return !(a == b); }

std::ostream& operator<<(std::ostream& out, const Rectangle& r) {
    return out << '(' << r.left() << ", " << r.bottom() << ")-("
               << r.right() << ", " << r.top() << ')';
}

std::istream& operator>>(std::istream& in, Rectangle& r) {
    int x1, y1, x2, y2;
    if (in >> x1 >> y1 >> x2 >> y2)
        r = Rectangle(x1, y1, x2, y2);
    return in;
}

} // namespace lab
