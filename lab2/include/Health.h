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

/// Одна запись в медицинской карте.
class MedicalEntry {
public:
    MedicalEntry(const Date& date, std::string diagnosis, std::string treatment,
                 std::string vetName, int severity);

    const Date& date() const { return date_; }
    const std::string& diagnosis() const { return diagnosis_; }
    const std::string& treatment() const { return treatment_; }
    const std::string& vetName() const { return vetName_; }
    int severity() const { return severity_; }

    bool isSerious() const;
    std::string summary() const;

private:
    Date date_;
    std::string diagnosis_;
    std::string treatment_;
    std::string vetName_;
    int severity_;
};

/// Медицинская карта животного.
class HealthRecord {
public:
    HealthRecord();

    const std::vector<MedicalEntry>& entries() const { return entries_; }
    bool isVaccinated() const { return vaccinated_; }
    bool isQuarantined() const { return quarantined_; }
    void setQuarantined(bool value) { quarantined_ = value; }

    void addEntry(const MedicalEntry& entry);
    /// Осмотр нужен, если его не было или прошло более 180 дней.
    bool needsCheckup(const Date& today) const;
    std::string lastDiagnosis() const;
    bool hasSeriousCondition() const;
    void vaccinate(const Date& date);
    bool vaccinationExpired(const Date& today) const;

private:
    std::vector<MedicalEntry> entries_;
    bool vaccinated_;
    bool quarantined_;
    std::optional<Date> vaccinationDate_;
};

/// Лекарство со складским остатком.
class Medication {
public:
    Medication(std::string name, double dosageMgPerKg, double stockMg, const Date& expiry);

    const std::string& name() const { return name_; }
    double stockMg() const { return stockMg_; }
    double dosageMgPerKg() const { return dosageMgPerKg_; }

    double doseFor(const Animal& animal) const;
    /// Выдать дозу для животного; возвращает выданное количество мг.
    double dispense(const Animal& animal, const Date& today);
    void restock(double mg);
    bool isExpired(const Date& today) const;

private:
    std::string name_;
    double dosageMgPerKg_;
    double stockMg_;
    Date expiry_;
};

}  // namespace ocean
