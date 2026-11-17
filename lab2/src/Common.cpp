#include "Common.h"

#include <cstdio>
#include <cstdlib>
#include <utility>

#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- Date

Date::Date(int day, int month, int year) : day_(day), month_(month), year_(year) {
    if (year < 1900 || year > 2200) {
        throw InvalidDateException("year " + std::to_string(year) + " out of range");
    }
    if (month < 1 || month > 12) {
        throw InvalidDateException("month " + std::to_string(month));
    }
    if (day < 1 || day > daysInMonth(month, year)) {
        throw InvalidDateException("day " + std::to_string(day));
    }
}

bool Date::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int Date::daysInMonth(int month, int year) {
    static const int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) {
        throw InvalidDateException("month " + std::to_string(month));
    }
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return kDays[month - 1];
}

long Date::toDayNumber() const {
    // Алгоритм days_from_civil (H. Hinnant).
    int y = year_ - (month_ <= 2 ? 1 : 0);
    const long era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned m = static_cast<unsigned>(month_);
    const unsigned doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + static_cast<unsigned>(day_) - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<long>(doe) - 719468;
}

Date Date::fromDayNumber(long z) {
    z += 719468;
    const long era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const long y = static_cast<long>(yoe) + era * 400;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    const unsigned d = doy - (153 * mp + 2) / 5 + 1;
    const unsigned m = mp < 10 ? mp + 3 : mp - 9;
    return Date(static_cast<int>(d), static_cast<int>(m), static_cast<int>(y + (m <= 2 ? 1 : 0)));
}

int Date::daysUntil(const Date& other) const {
    return static_cast<int>(other.toDayNumber() - toDayNumber());
}

bool Date::isBefore(const Date& other) const {
    return toDayNumber() < other.toDayNumber();
}

Date Date::addDays(int days) const {
    return fromDayNumber(toDayNumber() + days);
}

std::string Date::toString() const {
    char buffer[48];
    std::snprintf(buffer, sizeof(buffer), "%02d.%02d.%04d", day_, month_, year_);
    return buffer;
}

bool Date::operator==(const Date& other) const {
    return day_ == other.day_ && month_ == other.month_ && year_ == other.year_;
}

// ---------------------------------------------------------------- TimeSlot

TimeSlot::TimeSlot(int startMinute, int endMinute) : start_(startMinute), end_(endMinute) {
    if (startMinute < 0 || endMinute > 24 * 60 || startMinute >= endMinute) {
        throw InvalidParameterException("time slot must satisfy 0 <= start < end <= 1440");
    }
}

int TimeSlot::durationMinutes() const {
    return end_ - start_;
}

bool TimeSlot::overlaps(const TimeSlot& other) const {
    return start_ < other.end_ && other.start_ < end_;
}

bool TimeSlot::contains(int minute) const {
    return minute >= start_ && minute < end_;
}

std::string TimeSlot::toString() const {
    char buffer[48];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d-%02d:%02d", start_ / 60, start_ % 60,
                  end_ / 60, end_ % 60);
    return buffer;
}

// ---------------------------------------------------------------- Location

Location::Location(std::string zone, std::string building, int floor)
    : zone_(std::move(zone)), building_(std::move(building)), floor_(floor) {
    if (zone_.empty() || building_.empty()) {
        throw InvalidParameterException("location zone and building must be non-empty");
    }
}

bool Location::sameBuilding(const Location& other) const {
    return building_ == other.building_;
}

int Location::floorsBetween(const Location& other) const {
    return std::abs(floor_ - other.floor_);
}

std::string Location::toString() const {
    return zone_ + ", " + building_ + ", floor " + std::to_string(floor_);
}

}  // namespace ocean
