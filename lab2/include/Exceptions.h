/**
 * @file Exceptions.h
 * @brief Собственные исключения предметной области «Океанариум».
 */
#pragma once

#include <stdexcept>
#include <string>

namespace ocean {

/// Базовое исключение для всех ошибок океанариума.
class OceanariumException : public std::runtime_error {
public:
    explicit OceanariumException(const std::string& message)
        : std::runtime_error(message) {}
};

/// Некорректная календарная дата.
class InvalidDateException : public OceanariumException {
public:
    explicit InvalidDateException(const std::string& message)
        : OceanariumException("Invalid date: " + message) {}
};

/// Некорректный аргумент (отрицательный вес, пустое имя и т.п.).
class InvalidParameterException : public OceanariumException {
public:
    explicit InvalidParameterException(const std::string& message)
        : OceanariumException("Invalid parameter: " + message) {}
};

/// Параметры воды физически невозможны или не подходят обитателю.
class InvalidWaterParametersException : public OceanariumException {
public:
    explicit InvalidWaterParametersException(const std::string& message)
        : OceanariumException("Invalid water parameters: " + message) {}
};

/// В аквариуме нет места для нового животного.
class TankCapacityExceededException : public OceanariumException {
public:
    explicit TankCapacityExceededException(const std::string& message)
        : OceanariumException("Tank capacity exceeded: " + message) {}
};

/// Виды не могут жить вместе (хищник/жертва, разные требования к воде).
class IncompatibleSpeciesException : public OceanariumException {
public:
    explicit IncompatibleSpeciesException(const std::string& message)
        : OceanariumException("Incompatible species: " + message) {}
};

/// Животное с указанным идентификатором не найдено.
class AnimalNotFoundException : public OceanariumException {
public:
    explicit AnimalNotFoundException(int id)
        : OceanariumException("Animal not found: #" + std::to_string(id)) {}
};

/// Аквариум с указанным идентификатором не найден.
class TankNotFoundException : public OceanariumException {
public:
    explicit TankNotFoundException(int id)
        : OceanariumException("Tank not found: #" + std::to_string(id)) {}
};

/// Сотрудник с указанным идентификатором не найден.
class EmployeeNotFoundException : public OceanariumException {
public:
    explicit EmployeeNotFoundException(int id)
        : OceanariumException("Employee not found: #" + std::to_string(id)) {}
};

/// Недостаточно корма на складе.
class InsufficientFoodException : public OceanariumException {
public:
    explicit InsufficientFoodException(const std::string& message)
        : OceanariumException("Insufficient food: " + message) {}
};

/// Срок годности корма или лекарства истёк.
class ExpiredProductException : public OceanariumException {
public:
    explicit ExpiredProductException(const std::string& message)
        : OceanariumException("Expired product: " + message) {}
};

/// Товар или лекарство закончились.
class OutOfStockException : public OceanariumException {
public:
    explicit OutOfStockException(const std::string& message)
        : OceanariumException("Out of stock: " + message) {}
};

/// Склад переполнен.
class StorageFullException : public OceanariumException {
public:
    explicit StorageFullException(const std::string& message)
        : OceanariumException("Storage full: " + message) {}
};

/// Билет недействителен (использован, на другую дату, не тот тип).
class InvalidTicketException : public OceanariumException {
public:
    explicit InvalidTicketException(const std::string& message)
        : OceanariumException("Invalid ticket: " + message) {}
};

/// Билеты на указанную дату распроданы.
class TicketSoldOutException : public OceanariumException {
public:
    explicit TicketSoldOutException(const std::string& message)
        : OceanariumException("Tickets sold out: " + message) {}
};

/// Промокод недействителен или исчерпан.
class InvalidPromoCodeException : public OceanariumException {
public:
    explicit InvalidPromoCodeException(const std::string& code)
        : OceanariumException("Invalid promo code: " + code) {}
};

/// Недостаточно средств у посетителя или на счёте океанариума.
class InsufficientFundsException : public OceanariumException {
public:
    InsufficientFundsException(double required, double available)
        : OceanariumException("Insufficient funds: required " +
                              std::to_string(required) + ", available " +
                              std::to_string(available)) {}
};

/// Превышен лимит расходов бюджета.
class BudgetExceededException : public OceanariumException {
public:
    explicit BudgetExceededException(const std::string& message)
        : OceanariumException("Budget exceeded: " + message) {}
};

/// Пересечение смен, кормлений и т.п.
class ScheduleConflictException : public OceanariumException {
public:
    explicit ScheduleConflictException(const std::string& message)
        : OceanariumException("Schedule conflict: " + message) {}
};

/// Лицензия ветеринара просрочена.
class LicenseExpiredException : public OceanariumException {
public:
    explicit LicenseExpiredException(const std::string& message)
        : OceanariumException("License expired: " + message) {}
};

/// У сотрудника нет нужной квалификации или полномочий.
class EmployeeNotQualifiedException : public OceanariumException {
public:
    explicit EmployeeNotQualifiedException(const std::string& message)
        : OceanariumException("Employee not qualified: " + message) {}
};

/// Отказ оборудования (выключенный или забитый фильтр).
class EquipmentFailureException : public OceanariumException {
public:
    explicit EquipmentFailureException(const std::string& message)
        : OceanariumException("Equipment failure: " + message) {}
};

/// Датчик не откалиброван, показаниям нельзя доверять.
class SensorNotCalibratedException : public OceanariumException {
public:
    explicit SensorNotCalibratedException(const std::string& sensorId)
        : OceanariumException("Sensor not calibrated: " + sensorId) {}
};

/// Нет мест на экскурсию или шоу.
class EventFullException : public OceanariumException {
public:
    explicit EventFullException(const std::string& message)
        : OceanariumException("Event full: " + message) {}
};

/// Задача обслуживания не найдена.
class TaskNotFoundException : public OceanariumException {
public:
    explicit TaskNotFoundException(int id)
        : OceanariumException("Maintenance task not found: #" + std::to_string(id)) {}
};

/// Попытка выпустить животное из карантина досрочно.
class QuarantineViolationException : public OceanariumException {
public:
    explicit QuarantineViolationException(const std::string& message)
        : OceanariumException("Quarantine violation: " + message) {}
};

}  // namespace ocean
