#include <UnitTest++/UnitTest++.h>

#include "Common.h"
#include "Exceptions.h"

using namespace ocean;

SUITE(DateTests) {
    TEST(ValidDateStoresFields) {
        Date d(29, 2, 2024);
        CHECK_EQUAL(29, d.day());
        CHECK_EQUAL(2, d.month());
        CHECK_EQUAL(2024, d.year());
    }

    TEST(InvalidDayThrows) {
        CHECK_THROW(Date(29, 2, 2023), InvalidDateException);
        CHECK_THROW(Date(31, 4, 2026), InvalidDateException);
        CHECK_THROW(Date(0, 1, 2026), InvalidDateException);
    }

    TEST(InvalidMonthAndYearThrow) {
        CHECK_THROW(Date(1, 13, 2026), InvalidDateException);
        CHECK_THROW(Date(1, 1, 1800), InvalidDateException);
    }

    TEST(LeapYears) {
        CHECK(Date::isLeapYear(2000));
        CHECK(Date::isLeapYear(2024));
        CHECK(!Date::isLeapYear(1900));
        CHECK(!Date::isLeapYear(2026));
    }

    TEST(DaysInMonth) {
        CHECK_EQUAL(29, Date::daysInMonth(2, 2024));
        CHECK_EQUAL(28, Date::daysInMonth(2, 2026));
        CHECK_EQUAL(30, Date::daysInMonth(11, 2026));
    }

    TEST(DaysUntilAndIsBefore) {
        Date a(1, 1, 2026);
        Date b(1, 3, 2026);
        CHECK_EQUAL(59, a.daysUntil(b));
        CHECK_EQUAL(-59, b.daysUntil(a));
        CHECK(a.isBefore(b));
        CHECK(!b.isBefore(a));
    }

    TEST(AddDaysCrossesYear) {
        Date d = Date(30, 12, 2026).addDays(3);
        CHECK(d == Date(2, 1, 2027));
        CHECK(Date(1, 3, 2024).addDays(-1) == Date(29, 2, 2024));
    }

    TEST(DayNumberRoundTrip) {
        Date d(11, 11, 2026);
        CHECK(Date::fromDayNumber(d.toDayNumber()) == d);
        CHECK_EQUAL(0L, Date(1, 1, 1970).toDayNumber());
    }

    TEST(ToStringFormat) {
        CHECK_EQUAL("05.03.2026", Date(5, 3, 2026).toString());
    }
}

SUITE(TimeSlotTests) {
    TEST(DurationAndContains) {
        TimeSlot s(600, 690);
        CHECK_EQUAL(90, s.durationMinutes());
        CHECK(s.contains(600));
        CHECK(!s.contains(690));
    }

    TEST(Overlaps) {
        TimeSlot a(600, 720);
        CHECK(a.overlaps(TimeSlot(700, 800)));
        CHECK(!a.overlaps(TimeSlot(720, 800)));
    }

    TEST(InvalidSlotThrows) {
        CHECK_THROW(TimeSlot(700, 600), InvalidParameterException);
        CHECK_THROW(TimeSlot(-1, 60), InvalidParameterException);
        CHECK_THROW(TimeSlot(0, 1500), InvalidParameterException);
    }

    TEST(ToString) {
        CHECK_EQUAL("09:05-10:30", TimeSlot(545, 630).toString());
    }
}

SUITE(LocationTests) {
    TEST(SameBuildingAndFloors) {
        Location a("Tropics", "Main", 1);
        Location b("Arctic", "Main", 3);
        Location c("Arctic", "Annex", 0);
        CHECK(a.sameBuilding(b));
        CHECK(!a.sameBuilding(c));
        CHECK_EQUAL(2, a.floorsBetween(b));
    }

    TEST(EmptyZoneThrows) {
        CHECK_THROW(Location("", "Main", 1), InvalidParameterException);
    }

    TEST(ToString) {
        CHECK_EQUAL("Tropics, Main, floor 1", Location("Tropics", "Main", 1).toString());
    }
}
