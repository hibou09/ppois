#include "Health.h"

#include <utility>

#include "Animals.h"
#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- MedicalEntry

MedicalEntry::MedicalEntry(const Date& date, std::string diagnosis, std::string treatment,
                           std::string vetName, int severity)
    : date_(date), diagnosis_(std::move(diagnosis)), treatment_(std::move(treatment)),
      vetName_(std::move(vetName)), severity_(severity) {
    if (diagnosis_.empty()) {
        throw InvalidParameterException("diagnosis must be non-empty");
    }
    if (severity < 1 || severity > 5) {
        throw InvalidParameterException("severity must be in [1, 5]");
    }
}

bool MedicalEntry::isSerious() const {
    return severity_ >= 4;
}

std::string MedicalEntry::summary() const {
    return date_.toString() + ": " + diagnosis_ + " (severity " + std::to_string(severity_) +
           "), treatment: " + treatment_ + ", vet: " + vetName_;
}

// ---------------------------------------------------------------- HealthRecord

HealthRecord::HealthRecord() : vaccinated_(false), quarantined_(false) {}

void HealthRecord::addEntry(const MedicalEntry& entry) {
    if (!entries_.empty() && entry.date().isBefore(entries_.back().date())) {
        throw InvalidDateException("medical entries must be added in chronological order");
    }
    entries_.push_back(entry);
}

bool HealthRecord::needsCheckup(const Date& today) const {
    if (entries_.empty()) {
        return true;
    }
    return entries_.back().date().daysUntil(today) > 180;
}

std::string HealthRecord::lastDiagnosis() const {
    return entries_.empty() ? std::string() : entries_.back().diagnosis();
}

bool HealthRecord::hasSeriousCondition() const {
    return !entries_.empty() && entries_.back().isSerious();
}

void HealthRecord::vaccinate(const Date& date) {
    vaccinated_ = true;
    vaccinationDate_ = date;
}

bool HealthRecord::vaccinationExpired(const Date& today) const {
    return !vaccinated_ || vaccinationDate_->daysUntil(today) > 365;
}

// ---------------------------------------------------------------- Medication

Medication::Medication(std::string name, double dosageMgPerKg, double stockMg, const Date& expiry)
    : name_(std::move(name)), dosageMgPerKg_(dosageMgPerKg), stockMg_(stockMg), expiry_(expiry) {
    if (name_.empty() || dosageMgPerKg <= 0.0 || stockMg < 0.0) {
        throw InvalidParameterException("invalid medication values");
    }
}

double Medication::doseFor(const Animal& animal) const {
    return dosageMgPerKg_ * animal.weightKg();
}

double Medication::dispense(const Animal& animal, const Date& today) {
    if (isExpired(today)) {
        throw ExpiredProductException(name_ + " expired on " + expiry_.toString());
    }
    double dose = doseFor(animal);
    if (dose > stockMg_) {
        throw OutOfStockException(name_ + ": need " + std::to_string(dose) + " mg");
    }
    stockMg_ -= dose;
    return dose;
}

void Medication::restock(double mg) {
    if (mg <= 0.0) {
        throw InvalidParameterException("restock amount must be positive");
    }
    stockMg_ += mg;
}

bool Medication::isExpired(const Date& today) const {
    return expiry_.isBefore(today);
}

}  // namespace ocean
