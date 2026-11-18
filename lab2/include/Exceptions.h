/**
 * @file Exceptions.h
 * @brief Собственные исключения предметной области «Океанариум».
 */
#pragma once

#include <stdexcept>
#include <string>

namespace ocean {

/**
 * @brief Базовое исключение для всех ошибок океанариума.
 */
class OceanariumException : public std::runtime_error {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст ошибки.
     */
    explicit OceanariumException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Некорректная календарная дата.
 */
class InvalidDateException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit InvalidDateException(const std::string& message)
        : OceanariumException("Invalid date: " + message) {}
};

/**
 * @brief Некорректный аргумент (отрицательный вес, пустое имя и т.п.).
 */
class InvalidParameterException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit InvalidParameterException(const std::string& message)
        : OceanariumException("Invalid parameter: " + message) {}
};

/**
 * @brief Параметры воды физически невозможны или не подходят обитателю.
 */
class InvalidWaterParametersException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit InvalidWaterParametersException(const std::string& message)
        : OceanariumException("Invalid water parameters: " + message) {}
};

/**
 * @brief В аквариуме нет места для нового животного.
 */
class TankCapacityExceededException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit TankCapacityExceededException(const std::string& message)
        : OceanariumException("Tank capacity exceeded: " + message) {}
};

/**
 * @brief Виды не могут жить вместе (хищник/жертва, разные требования к воде).
 */
class IncompatibleSpeciesException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit IncompatibleSpeciesException(const std::string& message)
        : OceanariumException("Incompatible species: " + message) {}
};

/**
 * @brief Животное с указанным идентификатором не найдено.
 */
class AnimalNotFoundException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param id Идентификатор искомого объекта.
     */
    explicit AnimalNotFoundException(int id)
        : OceanariumException("Animal not found: #" + std::to_string(id)) {}
};

/**
 * @brief Аквариум с указанным идентификатором не найден.
 */
class TankNotFoundException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param id Идентификатор искомого объекта.
     */
    explicit TankNotFoundException(int id)
        : OceanariumException("Tank not found: #" + std::to_string(id)) {}
};

/**
 * @brief Сотрудник с указанным идентификатором не найден.
 */
class EmployeeNotFoundException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param id Идентификатор искомого объекта.
     */
    explicit EmployeeNotFoundException(int id)
        : OceanariumException("Employee not found: #" + std::to_string(id)) {}
};

/**
 * @brief Недостаточно корма на складе.
 */
class InsufficientFoodException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit InsufficientFoodException(const std::string& message)
        : OceanariumException("Insufficient food: " + message) {}
};

/**
 * @brief Срок годности корма или лекарства истёк.
 */
class ExpiredProductException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit ExpiredProductException(const std::string& message)
        : OceanariumException("Expired product: " + message) {}
};

/**
 * @brief Товар или лекарство закончились.
 */
class OutOfStockException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit OutOfStockException(const std::string& message)
        : OceanariumException("Out of stock: " + message) {}
};

/**
 * @brief Склад переполнен.
 */
class StorageFullException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit StorageFullException(const std::string& message)
        : OceanariumException("Storage full: " + message) {}
};

/**
 * @brief Билет недействителен (использован, на другую дату, не тот тип).
 */
class InvalidTicketException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit InvalidTicketException(const std::string& message)
        : OceanariumException("Invalid ticket: " + message) {}
};

/**
 * @brief Билеты на указанную дату распроданы.
 */
class TicketSoldOutException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit TicketSoldOutException(const std::string& message)
        : OceanariumException("Tickets sold out: " + message) {}
};

/**
 * @brief Промокод недействителен или исчерпан.
 */
class InvalidPromoCodeException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param code Код недействительного промокода.
     */
    explicit InvalidPromoCodeException(const std::string& code)
        : OceanariumException("Invalid promo code: " + code) {}
};

/**
 * @brief Недостаточно средств у посетителя или на счёте океанариума.
 */
class InsufficientFundsException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param required Требуемая сумма.
     * @param available Доступная сумма.
     */
    InsufficientFundsException(double required, double available)
        : OceanariumException("Insufficient funds: required " +
                              std::to_string(required) + ", available " +
                              std::to_string(available)) {}
};

/**
 * @brief Превышен лимит расходов бюджета.
 */
class BudgetExceededException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit BudgetExceededException(const std::string& message)
        : OceanariumException("Budget exceeded: " + message) {}
};

/**
 * @brief Пересечение смен, кормлений и т.п.
 */
class ScheduleConflictException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit ScheduleConflictException(const std::string& message)
        : OceanariumException("Schedule conflict: " + message) {}
};

/**
 * @brief Лицензия ветеринара просрочена.
 */
class LicenseExpiredException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit LicenseExpiredException(const std::string& message)
        : OceanariumException("License expired: " + message) {}
};

/**
 * @brief У сотрудника нет нужной квалификации или полномочий.
 */
class EmployeeNotQualifiedException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit EmployeeNotQualifiedException(const std::string& message)
        : OceanariumException("Employee not qualified: " + message) {}
};

/**
 * @brief Отказ оборудования (выключенный или забитый фильтр).
 */
class EquipmentFailureException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit EquipmentFailureException(const std::string& message)
        : OceanariumException("Equipment failure: " + message) {}
};

/**
 * @brief Датчик не откалиброван, показаниям нельзя доверять.
 */
class SensorNotCalibratedException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param sensorId Идентификатор датчика.
     */
    explicit SensorNotCalibratedException(const std::string& sensorId)
        : OceanariumException("Sensor not calibrated: " + sensorId) {}
};

/**
 * @brief Нет мест на экскурсию или шоу.
 */
class EventFullException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit EventFullException(const std::string& message)
        : OceanariumException("Event full: " + message) {}
};

/**
 * @brief Задача обслуживания не найдена.
 */
class TaskNotFoundException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param id Идентификатор искомого объекта.
     */
    explicit TaskNotFoundException(int id)
        : OceanariumException("Maintenance task not found: #" + std::to_string(id)) {}
};

/**
 * @brief Попытка выпустить животное из карантина досрочно.
 */
class QuarantineViolationException : public OceanariumException {
public:
    /**
     * @brief Создаёт исключение.
     * @param message Описание ошибки (добавляется к стандартному префиксу).
     */
    explicit QuarantineViolationException(const std::string& message)
        : OceanariumException("Quarantine violation: " + message) {}
};

}  // namespace ocean
