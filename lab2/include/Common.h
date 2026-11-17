/**
 * @file Common.h
 * @brief Базовые value-классы: дата, временной интервал, расположение.
 */
#pragma once

#include <string>

namespace ocean {

/// Календарная дата (григорианский календарь).
class Date {
public:
    Date(int day, int month, int year);

    int day() const { return day_; }
    int month() const { return month_; }
    int year() const { return year_; }

    static bool isLeapYear(int year);
    static int daysInMonth(int month, int year);
    static Date fromDayNumber(long dayNumber);

    /// Номер дня относительно 01.01.1970.
    long toDayNumber() const;
    /// Сколько дней от этой даты до other (может быть отрицательным).
    int daysUntil(const Date& other) const;
    bool isBefore(const Date& other) const;
    Date addDays(int days) const;
    std::string toString() const;

    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const { return !(*this == other); }

private:
    int day_;
    int month_;
    int year_;
};

/// Интервал времени внутри суток, в минутах от полуночи: [start, end).
class TimeSlot {
public:
    TimeSlot(int startMinute, int endMinute);

    int start() const { return start_; }
    int end() const { return end_; }

    int durationMinutes() const;
    bool overlaps(const TimeSlot& other) const;
    bool contains(int minute) const;
    std::string toString() const;

private:
    int start_;
    int end_;
};

/// Расположение объекта на территории океанариума.
class Location {
public:
    Location(std::string zone, std::string building, int floor);

    const std::string& zone() const { return zone_; }
    const std::string& building() const { return building_; }
    int floor() const { return floor_; }

    bool sameBuilding(const Location& other) const;
    int floorsBetween(const Location& other) const;
    std::string toString() const;

private:
    std::string zone_;
    std::string building_;
    int floor_;
};

}  // namespace ocean
