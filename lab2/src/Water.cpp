#include "Water.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- WaterParameters

WaterParameters::WaterParameters(double temperature, double salinity, double ph, double oxygen,
                                 double ammonia)
    : temperature_(temperature), salinity_(salinity), ph_(ph), oxygen_(oxygen), ammonia_(ammonia) {
    validate(salinity, ph, oxygen, ammonia);
    if (temperature < -2.0 || temperature > 40.0) {
        throw InvalidWaterParametersException("temperature out of range");
    }
}

void WaterParameters::validate(double salinity, double ph, double oxygen, double ammonia) {
    if (salinity < 0.0 || salinity > 50.0) {
        throw InvalidWaterParametersException("salinity must be in [0, 50] ppt");
    }
    if (ph < 0.0 || ph > 14.0) {
        throw InvalidWaterParametersException("pH must be in [0, 14]");
    }
    if (oxygen < 0.0) {
        throw InvalidWaterParametersException("oxygen must be non-negative");
    }
    if (ammonia < 0.0) {
        throw InvalidWaterParametersException("ammonia must be non-negative");
    }
}

void WaterParameters::adjustTemperature(double delta) {
    double next = temperature_ + delta;
    if (next < -2.0 || next > 40.0) {
        throw InvalidWaterParametersException("temperature would leave [-2, 40]");
    }
    temperature_ = next;
}

void WaterParameters::addAmmonia(double amount) {
    if (amount < 0.0) {
        throw InvalidParameterException("ammonia amount must be non-negative");
    }
    ammonia_ += amount;
}

void WaterParameters::reduceAmmonia(double fraction) {
    if (fraction < 0.0 || fraction > 1.0) {
        throw InvalidParameterException("fraction must be in [0, 1]");
    }
    ammonia_ *= (1.0 - fraction);
}

void WaterParameters::mixWith(const WaterParameters& fresh, double fraction) {
    if (fraction < 0.0 || fraction > 1.0) {
        throw InvalidParameterException("fraction must be in [0, 1]");
    }
    auto mix = [fraction](double a, double b) { return a * (1.0 - fraction) + b * fraction; };
    temperature_ = mix(temperature_, fresh.temperature_);
    salinity_ = mix(salinity_, fresh.salinity_);
    ph_ = mix(ph_, fresh.ph_);
    oxygen_ = mix(oxygen_, fresh.oxygen_);
    ammonia_ = mix(ammonia_, fresh.ammonia_);
}

bool WaterParameters::isToxic() const {
    return ammonia_ > 0.5 || oxygen_ < 3.0;
}

double WaterParameters::qualityIndex() const {
    double score = 100.0;
    score -= ammonia_ * 100.0;
    score -= std::fabs(ph_ - 8.1) * 10.0;
    if (oxygen_ < 6.0) {
        score -= (6.0 - oxygen_) * 10.0;
    }
    return std::clamp(score, 0.0, 100.0);
}

// ---------------------------------------------------------------- WaterRequirements

WaterRequirements::WaterRequirements(double minTemp, double maxTemp, double minSalinity,
                                     double maxSalinity, double minPh, double maxPh,
                                     double minOxygen)
    : minTemp_(minTemp), maxTemp_(maxTemp), minSalinity_(minSalinity), maxSalinity_(maxSalinity),
      minPh_(minPh), maxPh_(maxPh), minOxygen_(minOxygen) {
    if (minTemp > maxTemp || minSalinity > maxSalinity || minPh > maxPh) {
        throw InvalidParameterException("requirement minimum exceeds maximum");
    }
    if (minOxygen < 0.0) {
        throw InvalidParameterException("minimum oxygen must be non-negative");
    }
}

bool WaterRequirements::accepts(const WaterParameters& w) const {
    return w.temperature() >= minTemp_ && w.temperature() <= maxTemp_ &&
           w.salinity() >= minSalinity_ && w.salinity() <= maxSalinity_ && w.ph() >= minPh_ &&
           w.ph() <= maxPh_ && w.oxygen() >= minOxygen_ && !w.isToxic();
}

bool WaterRequirements::isSaltwater() const {
    return minSalinity_ >= 20.0;
}

bool WaterRequirements::overlaps(const WaterRequirements& o) const {
    return std::max(minTemp_, o.minTemp_) <= std::min(maxTemp_, o.maxTemp_) &&
           std::max(minSalinity_, o.minSalinity_) <= std::min(maxSalinity_, o.maxSalinity_) &&
           std::max(minPh_, o.minPh_) <= std::min(maxPh_, o.maxPh_);
}

WaterRequirements WaterRequirements::intersect(const WaterRequirements& o) const {
    if (!overlaps(o)) {
        throw IncompatibleSpeciesException("water requirements do not overlap");
    }
    return WaterRequirements(std::max(minTemp_, o.minTemp_), std::min(maxTemp_, o.maxTemp_),
                             std::max(minSalinity_, o.minSalinity_),
                             std::min(maxSalinity_, o.maxSalinity_), std::max(minPh_, o.minPh_),
                             std::min(maxPh_, o.maxPh_), std::max(minOxygen_, o.minOxygen_));
}

WaterParameters WaterRequirements::ideal() const {
    return WaterParameters((minTemp_ + maxTemp_) / 2.0, (minSalinity_ + maxSalinity_) / 2.0,
                           (minPh_ + maxPh_) / 2.0, minOxygen_ + 1.0, 0.0);
}

double WaterRequirements::deviation(const WaterParameters& w) const {
    auto outside = [](double v, double lo, double hi) {
        if (v < lo) return lo - v;
        if (v > hi) return v - hi;
        return 0.0;
    };
    double result = outside(w.temperature(), minTemp_, maxTemp_) +
                    outside(w.salinity(), minSalinity_, maxSalinity_) +
                    outside(w.ph(), minPh_, maxPh_);
    if (w.oxygen() < minOxygen_) {
        result += minOxygen_ - w.oxygen();
    }
    return result;
}

// ---------------------------------------------------------------- Sensor

Sensor::Sensor(std::string id, SensorType type, double minThreshold, double maxThreshold,
               bool calibrated)
    : id_(std::move(id)), type_(type), minThreshold_(minThreshold), maxThreshold_(maxThreshold),
      lastValue_(0.0), calibrated_(calibrated), readingsSinceCalibration_(0) {
    if (id_.empty()) {
        throw InvalidParameterException("sensor id must be non-empty");
    }
    if (minThreshold > maxThreshold) {
        throw InvalidParameterException("sensor min threshold exceeds max threshold");
    }
}

double Sensor::read(const WaterParameters& water) {
    if (!calibrated_) {
        throw SensorNotCalibratedException(id_);
    }
    switch (type_) {
        case SensorType::Temperature: lastValue_ = water.temperature(); break;
        case SensorType::Salinity: lastValue_ = water.salinity(); break;
        case SensorType::Ph: lastValue_ = water.ph(); break;
        case SensorType::Oxygen: lastValue_ = water.oxygen(); break;
        case SensorType::Ammonia: lastValue_ = water.ammonia(); break;
    }
    if (++readingsSinceCalibration_ >= kDriftLimit) {
        calibrated_ = false;
    }
    return lastValue_;
}

bool Sensor::isAlarm() const {
    return lastValue_ < minThreshold_ || lastValue_ > maxThreshold_;
}

void Sensor::calibrate() {
    calibrated_ = true;
    readingsSinceCalibration_ = 0;
}

// ---------------------------------------------------------------- FiltrationSystem

FiltrationSystem::FiltrationSystem(std::string model, double flowRateLph)
    : model_(std::move(model)), flowRateLph_(flowRateLph), filterWear_(0.0), active_(true) {
    if (flowRateLph <= 0.0) {
        throw InvalidParameterException("flow rate must be positive");
    }
}

void FiltrationSystem::runCycle(WaterParameters& water, double hours) {
    if (hours <= 0.0) {
        throw InvalidParameterException("filtration hours must be positive");
    }
    if (!active_) {
        throw EquipmentFailureException("filtration system " + model_ + " is off");
    }
    if (filterWear_ >= 100.0) {
        throw EquipmentFailureException("filter of " + model_ + " is clogged");
    }
    double efficiency = 1.0 - filterWear_ / 100.0;
    water.reduceAmmonia(std::min(0.9, 0.2 * hours * efficiency));
    filterWear_ = std::min(100.0, filterWear_ + hours * 0.5);
}

void FiltrationSystem::replaceFilter() {
    filterWear_ = 0.0;
}

bool FiltrationSystem::needsMaintenance() const {
    return filterWear_ >= 80.0;
}

double FiltrationSystem::turnoverPerHour(double volumeLiters) const {
    if (volumeLiters <= 0.0) {
        throw InvalidParameterException("volume must be positive");
    }
    return flowRateLph_ / volumeLiters;
}

// ---------------------------------------------------------------- HeatingSystem

HeatingSystem::HeatingSystem(double powerKw, double targetTemp)
    : powerKw_(powerKw), targetTemp_(targetTemp), active_(true) {
    if (powerKw <= 0.0) {
        throw InvalidParameterException("heater power must be positive");
    }
    setTargetTemperature(targetTemp);
}

void HeatingSystem::setTargetTemperature(double temp) {
    if (temp < 0.0 || temp > 35.0) {
        throw InvalidParameterException("target temperature must be in [0, 35]");
    }
    targetTemp_ = temp;
}

void HeatingSystem::regulate(WaterParameters& water, double hours) const {
    if (!active_ || hours <= 0.0) {
        return;
    }
    double maxDelta = powerKw_ * 0.5 * hours;
    double diff = targetTemp_ - water.temperature();
    water.adjustTemperature(std::clamp(diff, -maxDelta, maxDelta));
}

double HeatingSystem::energyConsumption(double hours) const {
    return active_ ? powerKw_ * hours : 0.0;
}

// ---------------------------------------------------------------- LightingSystem

LightingSystem::LightingSystem(double powerW, double dayLengthHours, double intensityLux)
    : powerW_(powerW), dayLengthHours_(0.0), intensityLux_(intensityLux), active_(true) {
    if (powerW <= 0.0 || intensityLux < 0.0) {
        throw InvalidParameterException("lighting power must be positive");
    }
    setDayLength(dayLengthHours);
}

void LightingSystem::setDayLength(double hours) {
    if (hours < 0.0 || hours > 16.0) {
        throw InvalidParameterException("day length must be in [0, 16] hours");
    }
    dayLengthHours_ = hours;
}

bool LightingSystem::isLightOn(int minuteOfDay) const {
    const int sunrise = 8 * 60;
    return active_ && minuteOfDay >= sunrise &&
           minuteOfDay < sunrise + static_cast<int>(dayLengthHours_ * 60.0);
}

double LightingSystem::dailyEnergyKwh() const {
    return active_ ? powerW_ * dayLengthHours_ / 1000.0 : 0.0;
}

void LightingSystem::dim(double percent) {
    if (percent < 0.0 || percent > 100.0) {
        throw InvalidParameterException("dim percent must be in [0, 100]");
    }
    intensityLux_ *= (1.0 - percent / 100.0);
}

}  // namespace ocean
