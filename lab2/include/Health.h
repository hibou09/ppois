/**
 * @file Health.h
 * @brief Медицинские записи, медкарта и лекарства.
 */
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Common.h"

namespace ocean {

class Animal;

/**
 * @brief Одна запись в медицинской карте.
 */
class MedicalEntry {
public:
    /**
     * @brief Создаёт запись о диагнозе и лечении.
     * @param date Дата осмотра.
     * @param diagnosis Диагноз (непустой).
     * @param treatment Назначенное лечение.
     * @param vetName Имя ветеринара.
     * @param severity Тяжесть от 1 до 5.
     * @throws InvalidParameterException если диагноз пуст или тяжесть вне [1, 5].
     */
    MedicalEntry(const Date& date, std::string diagnosis, std::string treatment,
                 std::string vetName, int severity);

    /// @brief Дата записи.
    const Date& date() const { return date_; }
    /// @brief Диагноз.
    const std::string& diagnosis() const { return diagnosis_; }
    /// @brief Назначенное лечение.
    const std::string& treatment() const { return treatment_; }
    /// @brief Имя ветеринара.
    const std::string& vetName() const { return vetName_; }
    /// @brief Тяжесть состояния (1..5).
    int severity() const { return severity_; }

    /**
     * @brief Серьёзное ли состояние.
     * @return true, если тяжесть не меньше 4.
     */
    bool isSerious() const;
    /**
     * @brief Краткое текстовое описание записи.
     * @return Строка с датой, диагнозом, тяжестью, лечением и ветеринаром.
     */
    std::string summary() const;

private:
    Date date_;             ///< Дата записи.
    std::string diagnosis_; ///< Диагноз.
    std::string treatment_; ///< Лечение.
    std::string vetName_;   ///< Имя ветеринара.
    int severity_;          ///< Тяжесть состояния (1..5).
};

/**
 * @brief Медицинская карта животного.
 * @details Хранит хронологический список записей, данные о вакцинации и карантине.
 */
class HealthRecord {
public:
    /// @brief Создаёт пустую карту: без записей, не вакцинирована, не на карантине.
    HealthRecord();

    /// @brief Все записи карты в хронологическом порядке.
    const std::vector<MedicalEntry>& entries() const { return entries_; }
    /// @brief Вакцинировано ли животное.
    bool isVaccinated() const { return vaccinated_; }
    /// @brief Находится ли животное на карантине.
    bool isQuarantined() const { return quarantined_; }
    /// @brief Устанавливает или снимает признак карантина.
    void setQuarantined(bool value) { quarantined_ = value; }

    /**
     * @brief Добавляет запись в карту.
     * @param entry Новая запись.
     * @throws InvalidDateException если запись раньше последней добавленной.
     */
    void addEntry(const MedicalEntry& entry);
    /**
     * @brief Осмотр нужен, если его не было или прошло более 180 дней.
     * @param today Текущая дата.
     * @return true, если нужен плановый осмотр.
     */
    bool needsCheckup(const Date& today) const;
    /**
     * @brief Последний диагноз.
     * @return Диагноз последней записи или пустая строка, если записей нет.
     */
    std::string lastDiagnosis() const;
    /**
     * @brief Есть ли серьёзное состояние.
     * @return true, если последняя запись серьёзная.
     */
    bool hasSeriousCondition() const;
    /**
     * @brief Фиксирует вакцинацию.
     * @param date Дата вакцинации.
     */
    void vaccinate(const Date& date);
    /**
     * @brief Истёк ли срок действия вакцинации.
     * @param today Текущая дата.
     * @return true, если животное не вакцинировано или с вакцинации прошло более 365 дней.
     */
    bool vaccinationExpired(const Date& today) const;

private:
    std::vector<MedicalEntry> entries_;      ///< Записи карты.
    bool vaccinated_;                        ///< Признак вакцинации.
    bool quarantined_;                       ///< Признак карантина.
    std::optional<Date> vaccinationDate_;    ///< Дата последней вакцинации.
};

/**
 * @brief Лекарство со складским остатком.
 */
class Medication {
public:
    /**
     * @brief Создаёт лекарство.
     * @param name Название (непустое).
     * @param dosageMgPerKg Дозировка, мг на кг массы животного (положительная).
     * @param stockMg Остаток на складе, мг (неотрицательный).
     * @param expiry Срок годности.
     * @throws InvalidParameterException если название пусто, дозировка не положительна
     *         или остаток отрицателен.
     */
    Medication(std::string name, double dosageMgPerKg, double stockMg, const Date& expiry);

    /// @brief Название лекарства.
    const std::string& name() const { return name_; }
    /// @brief Остаток на складе, мг.
    double stockMg() const { return stockMg_; }
    /// @brief Дозировка, мг на кг массы.
    double dosageMgPerKg() const { return dosageMgPerKg_; }

    /**
     * @brief Рассчитывает дозу для животного.
     * @param animal Животное.
     * @return Доза, мг (дозировка × масса).
     */
    double doseFor(const Animal& animal) const;
    /**
     * @brief Выдаёт дозу для животного.
     * @param animal Животное.
     * @param today Текущая дата.
     * @return Выданное количество, мг.
     * @throws ExpiredProductException если срок годности истёк.
     * @throws OutOfStockException если остатка не хватает на дозу.
     */
    double dispense(const Animal& animal, const Date& today);
    /**
     * @brief Пополняет запас.
     * @param mg Добавляемое количество, мг.
     * @throws InvalidParameterException если mg не положительно.
     */
    void restock(double mg);
    /**
     * @brief Просрочено ли лекарство.
     * @param today Текущая дата.
     * @return true, если срок годности раньше текущей даты.
     */
    bool isExpired(const Date& today) const;

private:
    std::string name_;       ///< Название.
    double dosageMgPerKg_;   ///< Дозировка, мг/кг.
    double stockMg_;         ///< Остаток на складе, мг.
    Date expiry_;            ///< Срок годности.
};

}  // namespace ocean
