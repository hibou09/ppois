#include "Animals.h"

#include <algorithm>
#include <utility>

#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- Species

Species::Species(std::string name, std::string latinName, Diet diet,
                 WaterRequirements requirements, bool predatory, ConservationStatus status)
    : name_(std::move(name)), latinName_(std::move(latinName)), diet_(diet),
      requirements_(requirements), predatory_(predatory), status_(status) {
    if (name_.empty()) {
        throw InvalidParameterException("species name must be non-empty");
    }
}

bool Species::isEndangered() const {
    return status_ == ConservationStatus::Endangered ||
           status_ == ConservationStatus::CriticallyEndangered;
}

bool Species::canCoexistWith(const Species& other) const {
    if (!requirements_.overlaps(other.requirements_)) {
        return false;
    }
    if (name_ == other.name_) {
        return true;
    }
    // Хищник не уживается с нехищными видами.
    return predatory_ == other.predatory_;
}

// ---------------------------------------------------------------- Animal

Animal::Animal(int id, std::string name, Species species, int ageYears, double weightKg)
    : id_(id), name_(std::move(name)), species_(std::move(species)), ageYears_(ageYears),
      weightKg_(weightKg), hunger_(0.0) {
    if (id <= 0) {
        throw InvalidParameterException("animal id must be positive");
    }
    if (name_.empty()) {
        throw InvalidParameterException("animal name must be non-empty");
    }
    if (ageYears < 0 || weightKg <= 0.0) {
        throw InvalidParameterException("animal age/weight invalid");
    }
}

std::string Animal::describe() const {
    return kind() + " " + name_ + " (" + species_.name() + ", " + std::to_string(ageYears_) +
           " y.o.)";
}

void Animal::passDay() {
    increaseHunger(40.0);
}

void Animal::increaseHunger(double amount) {
    hunger_ = std::clamp(hunger_ + amount, 0.0, 100.0);
}

void Animal::feed(FoodItem& food, double kg, const Date& today) {
    if (kg <= 0.0) {
        throw InvalidParameterException("food amount must be positive");
    }
    food.consume(kg, today);
    if (hunger_ <= 0.0) {
        weightKg_ += kg * 0.05;  // перекорм
    }
    hunger_ = std::max(0.0, hunger_ - kg / dailyFoodKg() * 100.0);
}

bool Animal::isHungry() const {
    return hunger_ >= 50.0;
}

void Animal::celebrateBirthday() {
    ++ageYears_;
}

bool Animal::canLiveIn(const WaterParameters& water) const {
    return species_.requirements().accepts(water);
}

bool Animal::canShareTankWith(const Animal& other) const {
    return species_.canCoexistWith(other.species_);
}

// ---------------------------------------------------------------- Fish

Fish::Fish(int id, std::string name, Species species, int ageYears, double weightKg,
           double lengthCm, bool schooling)
    : Animal(id, std::move(name), std::move(species), ageYears, weightKg), lengthCm_(lengthCm),
      schooling_(schooling) {
    if (lengthCm <= 0.0) {
        throw InvalidParameterException("fish length must be positive");
    }
}

double Fish::dailyFoodKg() const {
    return weightKg_ * 0.03;
}

double Fish::swimSpeed() const {
    return lengthCm_ * 0.02;
}

void Fish::grow(double cm) {
    if (cm <= 0.0) {
        throw InvalidParameterException("growth must be positive");
    }
    lengthCm_ += cm;
}

// ---------------------------------------------------------------- Shark

Shark::Shark(int id, std::string name, Species species, int ageYears, double weightKg,
             double lengthCm, double biteForceN)
    : Fish(id, std::move(name), std::move(species), ageYears, weightKg, lengthCm, false),
      biteForceN_(biteForceN), teethLost_(0) {
    if (biteForceN <= 0.0) {
        throw InvalidParameterException("bite force must be positive");
    }
}

double Shark::dailyFoodKg() const {
    return weightKg_ * 0.02;
}

bool Shark::isDangerousTo(const Animal& other) const {
    return other.id() != id_ && other.kind() != "Shark" && other.weightKg() < weightKg_ * 0.5;
}

void Shark::loseTeeth(int count) {
    if (count <= 0) {
        throw InvalidParameterException("teeth count must be positive");
    }
    teethLost_ += count;
}

// ---------------------------------------------------------------- Ray

Ray::Ray(int id, std::string name, Species species, int ageYears, double weightKg,
         double wingspanCm, bool venomous)
    : Fish(id, std::move(name), std::move(species), ageYears, weightKg, wingspanCm, false),
      wingspanCm_(wingspanCm), venomous_(venomous), barbClipped_(false) {}

bool Ray::isSafeForTouch() const {
    return !venomous_ || barbClipped_;
}

void Ray::clipBarb() {
    if (!venomous_) {
        throw InvalidParameterException(name_ + " has no venomous barb");
    }
    barbClipped_ = true;
}

// ---------------------------------------------------------------- Turtle

Turtle::Turtle(int id, std::string name, Species species, int ageYears, double weightKg,
               double shellLengthCm)
    : Animal(id, std::move(name), std::move(species), ageYears, weightKg),
      shellLengthCm_(shellLengthCm), baskingHoursToday_(0.0) {
    if (shellLengthCm <= 0.0) {
        throw InvalidParameterException("shell length must be positive");
    }
}

double Turtle::dailyFoodKg() const {
    return weightKg_ * 0.015;
}

void Turtle::passDay() {
    Animal::passDay();
    baskingHoursToday_ = 0.0;
}

void Turtle::bask(double hours) {
    if (hours <= 0.0 || baskingHoursToday_ + hours > 12.0) {
        throw InvalidParameterException("basking hours must be positive and <= 12 per day");
    }
    baskingHoursToday_ += hours;
}

bool Turtle::needsBasking() const {
    return baskingHoursToday_ < 2.0;
}

int Turtle::remainingLifespan() const {
    return std::max(0, kLifespanYears - ageYears_);
}

// ---------------------------------------------------------------- Octopus

Octopus::Octopus(int id, std::string name, Species species, int ageYears, double weightKg,
                 int intelligence)
    : Animal(id, std::move(name), std::move(species), ageYears, weightKg), armCount_(8),
      intelligence_(intelligence), escapeAttempts_(0) {
    if (intelligence < 0 || intelligence > 100) {
        throw InvalidParameterException("intelligence must be in [0, 100]");
    }
}

double Octopus::dailyFoodKg() const {
    return weightKg_ * 0.04;
}

bool Octopus::solvePuzzle(int difficulty) {
    if (difficulty < 1 || difficulty > 10) {
        throw InvalidParameterException("puzzle difficulty must be in [1, 10]");
    }
    bool solved = intelligence_ >= difficulty * 10;
    if (solved) {
        intelligence_ = std::min(100, intelligence_ + 2);
    }
    return solved;
}

bool Octopus::attemptEscape(bool lidSecured) {
    ++escapeAttempts_;
    return !lidSecured;
}

void Octopus::loseArm() {
    if (armCount_ == 0) {
        throw InvalidParameterException(name_ + " has no arms left");
    }
    --armCount_;
}

void Octopus::regenerateArm() {
    if (armCount_ < 8) {
        ++armCount_;
    }
}

// ---------------------------------------------------------------- Jellyfish

Jellyfish::Jellyfish(int id, std::string name, Species species, int ageYears, double weightKg,
                     double bellDiameterCm, bool stinging)
    : Animal(id, std::move(name), std::move(species), ageYears, weightKg),
      bellDiameterCm_(bellDiameterCm), stinging_(stinging) {
    if (bellDiameterCm <= 0.0) {
        throw InvalidParameterException("bell diameter must be positive");
    }
}

double Jellyfish::dailyFoodKg() const {
    return std::max(0.001, bellDiameterCm_ * 0.0005);
}

double Jellyfish::pulseRate(double waterTemperature) const {
    return std::max(10.0, 30.0 + (waterTemperature - 20.0) * 2.0);
}

bool Jellyfish::isSafeForTouch() const {
    return !stinging_;
}

// ---------------------------------------------------------------- Penguin

Penguin::Penguin(int id, std::string name, Species species, int ageYears, double weightKg,
                 std::string colony)
    : Animal(id, std::move(name), std::move(species), ageYears, weightKg),
      colony_(std::move(colony)), swimHoursToday_(0.0), molting_(false) {}

double Penguin::dailyFoodKg() const {
    return weightKg_ * (molting_ ? 0.15 : 0.1);
}

void Penguin::passDay() {
    Animal::passDay();
    swimHoursToday_ = 0.0;
}

void Penguin::swim(double hours) {
    if (molting_) {
        throw InvalidParameterException(name_ + " is molting and cannot swim");
    }
    if (hours <= 0.0) {
        throw InvalidParameterException("swim hours must be positive");
    }
    swimHoursToday_ += hours;
    increaseHunger(hours * 5.0);
}

void Penguin::startMolt() {
    molting_ = true;
}

void Penguin::finishMolt() {
    molting_ = false;
}

bool Penguin::hadEnoughExercise() const {
    return molting_ || swimHoursToday_ >= 1.0;
}

// ---------------------------------------------------------------- Seal

Seal::Seal(int id, std::string name, Species species, int ageYears, double weightKg,
           double blubberCm)
    : Animal(id, std::move(name), std::move(species), ageYears, weightKg),
      blubberCm_(blubberCm), performances_(0) {}

double Seal::dailyFoodKg() const {
    return weightKg_ * 0.06;
}

bool Seal::learnTrick(const std::string& trick) {
    if (trick.empty()) {
        throw InvalidParameterException("trick name must be non-empty");
    }
    if (knowsTrick(trick)) {
        return false;
    }
    tricks_.push_back(trick);
    return true;
}

bool Seal::knowsTrick(const std::string& trick) const {
    return std::find(tricks_.begin(), tricks_.end(), trick) != tricks_.end();
}

bool Seal::perform(const std::string& trick) {
    if (!knowsTrick(trick)) {
        return false;
    }
    ++performances_;
    increaseHunger(5.0);
    return true;
}

// ---------------------------------------------------------------- Coral

Coral::Coral(std::string speciesName, double sizeCm2, double healthPercent)
    : speciesName_(std::move(speciesName)), sizeCm2_(sizeCm2), healthPercent_(healthPercent),
      bleached_(healthPercent < 50.0) {
    if (speciesName_.empty() || sizeCm2 <= 0.0 || healthPercent < 0.0 || healthPercent > 100.0) {
        throw InvalidParameterException("invalid coral values");
    }
}

double Coral::grow(const WaterParameters& water) {
    if (water.temperature() > kBleachingTemperature) {
        healthPercent_ = std::max(0.0, healthPercent_ - 10.0);
        if (healthPercent_ < 50.0) {
            bleached_ = true;
        }
    } else if (!bleached_) {
        sizeCm2_ *= 1.02;
    }
    return sizeCm2_;
}

Coral Coral::fragment(double cm2) {
    if (cm2 <= 0.0 || cm2 >= sizeCm2_) {
        throw InvalidParameterException("fragment must be smaller than the colony");
    }
    sizeCm2_ -= cm2;
    return Coral(speciesName_, cm2, healthPercent_);
}

void Coral::recover() {
    healthPercent_ = std::min(100.0, healthPercent_ + 20.0);
    if (healthPercent_ >= 50.0) {
        bleached_ = false;
    }
}

}  // namespace ocean
