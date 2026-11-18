/**
 * @file Common.h
 * @brief Базовые value-классы: дата, временной интервал, расположение.
 */
#pragma once

#include <string>

namespace ocean {

/**
 * @brief Календарная дата (григорианский календарь).
 * @details Допустимы годы 1900–2200. Объект неизменяем: операции возвращают новые даты.
 */
class Date {
public:
    /**
     * @brief Создаёт дату с проверкой корректности.
     * @param day День месяца (1..число дней в месяце).
     * @param month Месяц (1..12).
     * @param year Год (1900..2200).
     * @throws InvalidDateException если год, месяц или день вне допустимого диапазона.
     */
    Date(int day, int month, int year);

    /// @brief День месяца.
    int day() const { return day_; }
    /// @brief Номер месяца (1..12).
    int month() const { return month_; }
    /// @brief Год.
    int year() const { return year_; }

    /**
     * @brief Проверяет, является ли год високосным.
     * @param year Проверяемый год.
     * @return true, если год високосный.
     */
    static bool isLeapYear(int year);
    /**
     * @brief Возвращает число дней в месяце.
     * @param month Номер месяца (1..12).
     * @param year Год (нужен для февраля).
     * @return Количество дней в месяце.
     * @throws InvalidDateException если месяц вне диапазона 1..12.
     */
    static int daysInMonth(int month, int year);
    /**
     * @brief Строит дату по номеру дня относительно 01.01.1970.
     * @param dayNumber Номер дня (может быть отрицательным).
     * @return Соответствующая календарная дата.
     * @throws InvalidDateException если результат выходит за допустимые годы.
     */
    static Date fromDayNumber(long dayNumber);

    /**
     * @brief Номер дня относительно 01.01.1970.
     * @return Количество суток от 01.01.1970 до этой даты.
     */
    long toDayNumber() const;
    /**
     * @brief Сколько дней от этой даты до other.
     * @param other Целевая дата.
     * @return Число дней (отрицательное, если other раньше).
     */
    int daysUntil(const Date& other) const;
    /**
     * @brief Проверяет, что эта дата раньше другой.
     * @param other Сравниваемая дата.
     * @return true, если эта дата строго раньше other.
     */
    bool isBefore(const Date& other) const;
    /**
     * @brief Сдвигает дату на заданное число дней.
     * @param days Число дней (может быть отрицательным).
     * @return Новая дата.
     * @throws InvalidDateException если результат выходит за допустимые годы.
     */
    Date addDays(int days) const;
    /**
     * @brief Текстовое представление даты.
     * @return Строка формата ДД.ММ.ГГГГ.
     */
    std::string toString() const;

    /**
     * @brief Сравнение дат на равенство.
     * @param other Сравниваемая дата.
     * @return true, если совпадают день, месяц и год.
     */
    bool operator==(const Date& other) const;
    /**
     * @brief Сравнение дат на неравенство.
     * @param other Сравниваемая дата.
     * @return true, если даты различаются.
     */
    bool operator!=(const Date& other) const { return !(*this == other); }

private:
    int day_;    ///< День месяца.
    int month_;  ///< Месяц (1..12).
    int year_;   ///< Год.
};

/**
 * @brief Интервал времени внутри суток, в минутах от полуночи: [start, end).
 */
class TimeSlot {
public:
    /**
     * @brief Создаёт интервал.
     * @param startMinute Начало, минуты от полуночи (включительно).
     * @param endMinute Конец, минуты от полуночи (не включительно).
     * @throws InvalidParameterException если не выполняется 0 <= start < end <= 1440.
     */
    TimeSlot(int startMinute, int endMinute);

    /// @brief Начало интервала в минутах от полуночи.
    int start() const { return start_; }
    /// @brief Конец интервала в минутах от полуночи (не включительно).
    int end() const { return end_; }

    /**
     * @brief Длительность интервала.
     * @return Длительность в минутах.
     */
    int durationMinutes() const;
    /**
     * @brief Проверяет пересечение с другим интервалом.
     * @param other Другой интервал.
     * @return true, если интервалы имеют общую часть.
     */
    bool overlaps(const TimeSlot& other) const;
    /**
     * @brief Проверяет, попадает ли момент времени в интервал.
     * @param minute Момент, минуты от полуночи.
     * @return true, если start <= minute < end.
     */
    bool contains(int minute) const;
    /**
     * @brief Текстовое представление интервала.
     * @return Строка формата ЧЧ:ММ-ЧЧ:ММ.
     */
    std::string toString() const;

private:
    int start_;  ///< Начало интервала, минуты от полуночи.
    int end_;    ///< Конец интервала, минуты от полуночи.
};

/**
 * @brief Расположение объекта на территории океанариума.
 */
class Location {
public:
    /**
     * @brief Создаёт расположение.
     * @param zone Название зоны.
     * @param building Название здания.
     * @param floor Номер этажа.
     * @throws InvalidParameterException если зона или здание пусты.
     */
    Location(std::string zone, std::string building, int floor);

    /// @brief Название зоны.
    const std::string& zone() const { return zone_; }
    /// @brief Название здания.
    const std::string& building() const { return building_; }
    /// @brief Номер этажа.
    int floor() const { return floor_; }

    /**
     * @brief Проверяет, находятся ли объекты в одном здании.
     * @param other Другое расположение.
     * @return true, если названия зданий совпадают.
     */
    bool sameBuilding(const Location& other) const;
    /**
     * @brief Число этажей между расположениями.
     * @param other Другое расположение.
     * @return Модуль разности номеров этажей.
     */
    int floorsBetween(const Location& other) const;
    /**
     * @brief Текстовое представление расположения.
     * @return Строка «зона, здание, floor N».
     */
    std::string toString() const;

private:
    std::string zone_;      ///< Название зоны.
    std::string building_;  ///< Название здания.
    int floor_;             ///< Номер этажа.
};

}  // namespace ocean
