#include "Tanks.h"

#include <algorithm>
#include <set>
#include <utility>

#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- Tank

Tank::Tank(int id, std::string name, double volumeLiters, WaterParameters water,
           Location location, int maxAnimals)
    : id_(id), name_(std::move(name)), volumeLiters_(volumeLiters), water_(water),
      location_(std::move(location)), maxAnimals_(maxAnimals), open_(true),
      filtration_("Standard-" + std::to_string(id), volumeLiters * 2.0),
      heating_(5.0, water.temperature()) {
    if (id <= 0 || name_.empty()) {
        throw InvalidParameterException("tank needs positive id and name");
    }
    if (volumeLiters <= 0.0 || maxAnimals <= 0) {
        throw InvalidParameterException("tank volume and capacity must be positive");
    }
}

bool Tank::canHost(const Animal& animal) const {
    return animal.canLiveIn(water_);
}

void Tank::addAnimal(std::shared_ptr<Animal> animal) {
    if (!animal) {
        throw InvalidParameterException("animal must not be null");
    }
    if (hasAnimal(animal->id())) {
        throw InvalidParameterException("animal #" + std::to_string(animal->id()) +
                                        " already lives in " + name_);
    }
    if (animalCount() >= maxAnimals_) {
        throw TankCapacityExceededException(name_ + " holds at most " +
                                            std::to_string(maxAnimals_) + " animals");
    }
    if (!animal->canLiveIn(water_)) {
        throw InvalidWaterParametersException("water in " + name_ + " is unsuitable for " +
                                              animal->name());
    }
    if (!canHost(*animal)) {
        throw IncompatibleSpeciesException(type() + " cannot host " + animal->describe());
    }
    for (const auto& resident : animals_) {
        if (!animal->canShareTankWith(*resident)) {
            throw IncompatibleSpeciesException(animal->name() + " cannot live with " +
                                               resident->name());
        }
    }
    animals_.push_back(std::move(animal));
}

std::shared_ptr<Animal> Tank::removeAnimal(int animalId) {
    auto it = std::find_if(animals_.begin(), animals_.end(),
                           [animalId](const auto& a) { return a->id() == animalId; });
    if (it == animals_.end()) {
        throw AnimalNotFoundException(animalId);
    }
    auto animal = *it;
    animals_.erase(it);
    return animal;
}

Animal& Tank::findAnimal(int animalId) const {
    for (const auto& a : animals_) {
        if (a->id() == animalId) {
            return *a;
        }
    }
    throw AnimalNotFoundException(animalId);
}

bool Tank::hasAnimal(int animalId) const {
    return std::any_of(animals_.begin(), animals_.end(),
                       [animalId](const auto& a) { return a->id() == animalId; });
}

int Tank::animalCount() const {
    return static_cast<int>(animals_.size());
}

double Tank::totalBiomass() const {
    double total = 0.0;
    for (const auto& a : animals_) {
        total += a->weightKg();
    }
    return total;
}

double Tank::stockingDensity() const {
    return totalBiomass() / (volumeLiters_ / 1000.0);
}

double Tank::dailyFoodRequirement() const {
    double total = 0.0;
    for (const auto& a : animals_) {
        total += a->dailyFoodKg();
    }
    return total;
}

int Tank::speciesCount() const {
    std::set<std::string> names;
    for (const auto& a : animals_) {
        names.insert(a->species().name());
    }
    return static_cast<int>(names.size());
}

int Tank::hungryAnimals() const {
    return static_cast<int>(std::count_if(animals_.begin(), animals_.end(),
                                          [](const auto& a) { return a->isHungry(); }));
}

void Tank::performWaterChange(double fraction) {
    if (fraction <= 0.0 || fraction > 0.5) {
        throw InvalidParameterException("water change fraction must be in (0, 0.5]");
    }
    WaterParameters fresh(water_.temperature(), water_.salinity(), water_.ph(), water_.oxygen(),
                          0.0);
    water_.mixWith(fresh, fraction);
}

void Tank::runLifeSupport(double hours) {
    if (hours <= 0.0) {
        throw InvalidParameterException("hours must be positive");
    }
    water_.addAmmonia(totalBiomass() * hours * 0.002 / (volumeLiters_ / 1000.0));
    heating_.regulate(water_, hours);
    filtration_.runCycle(water_, hours);
}

bool Tank::needsCleaning() const {
    return water_.ammonia() >= 0.2;
}

bool Tank::isHealthy() const {
    return std::all_of(animals_.begin(), animals_.end(),
                       [this](const auto& a) { return a->canLiveIn(water_); });
}

// ---------------------------------------------------------------- CoralReefTank

CoralReefTank::CoralReefTank(int id, std::string name, double volumeLiters, WaterParameters water,
                             Location location, int maxAnimals, double waveIntensity)
    : Tank(id, std::move(name), volumeLiters, water, std::move(location), maxAnimals),
      waveIntensity_(waveIntensity) {
    if (waveIntensity < 0.0 || waveIntensity > 10.0) {
        throw InvalidParameterException("wave intensity must be in [0, 10]");
    }
}

bool CoralReefTank::canHost(const Animal& animal) const {
    // Крупные акулы разрушают риф.
    if (animal.kind() == "Shark" && animal.weightKg() > 30.0) {
        return false;
    }
    return Tank::canHost(animal);
}

void CoralReefTank::addCoral(const Coral& coral) {
    corals_.push_back(coral);
}

double CoralReefTank::totalCoralArea() const {
    double total = 0.0;
    for (const auto& c : corals_) {
        total += c.size();
    }
    return total;
}

void CoralReefTank::growCorals() {
    for (auto& c : corals_) {
        c.grow(water_);
    }
}

int CoralReefTank::bleachedCount() const {
    return static_cast<int>(std::count_if(corals_.begin(), corals_.end(),
                                          [](const Coral& c) { return c.isBleached(); }));
}

bool CoralReefTank::bleachingRisk() const {
    return water_.temperature() > Coral::kBleachingTemperature - 0.5;
}

// ---------------------------------------------------------------- OpenOceanTank

OpenOceanTank::OpenOceanTank(int id, std::string name, double volumeLiters, WaterParameters water,
                             Location location, int maxAnimals, double depthM,
                             double tunnelLengthM)
    : Tank(id, std::move(name), volumeLiters, water, std::move(location), maxAnimals),
      depthM_(depthM), tunnelLengthM_(tunnelLengthM) {
    if (depthM <= 0.0 || tunnelLengthM < 0.0) {
        throw InvalidParameterException("depth must be positive, tunnel length non-negative");
    }
}

bool OpenOceanTank::hasTunnel() const {
    return tunnelLengthM_ > 0.0;
}

int OpenOceanTank::tunnelVisitorCapacity() const {
    return static_cast<int>(tunnelLengthM_ * 3.0);
}

bool OpenOceanTank::requiresDiverCertification() const {
    return depthM_ > 5.0;
}

int OpenOceanTank::predatorCount() const {
    return static_cast<int>(std::count_if(animals_.begin(), animals_.end(), [](const auto& a) {
        return a->species().isPredatory();
    }));
}

// ---------------------------------------------------------------- TouchPool

TouchPool::TouchPool(int id, std::string name, double volumeLiters, WaterParameters water,
                     Location location, int maxAnimals, int maxTouchesPerDay)
    : Tank(id, std::move(name), volumeLiters, water, std::move(location), maxAnimals),
      maxTouchesPerDay_(maxTouchesPerDay), touchesToday_(0) {
    if (maxTouchesPerDay <= 0) {
        throw InvalidParameterException("max touches per day must be positive");
    }
}

bool TouchPool::canHost(const Animal& animal) const {
    if (!Tank::canHost(animal)) {
        return false;
    }
    if (const auto* ray = dynamic_cast<const Ray*>(&animal)) {
        return ray->isSafeForTouch();
    }
    if (const auto* jelly = dynamic_cast<const Jellyfish*>(&animal)) {
        return jelly->isSafeForTouch();
    }
    return dynamic_cast<const Turtle*>(&animal) != nullptr;
}

bool TouchPool::allowTouch() {
    if (!open_ || touchesToday_ >= maxTouchesPerDay_) {
        return false;
    }
    ++touchesToday_;
    return true;
}

int TouchPool::remainingTouches() const {
    return maxTouchesPerDay_ - touchesToday_;
}

void TouchPool::resetDay() {
    touchesToday_ = 0;
}

// ---------------------------------------------------------------- QuarantineTank

QuarantineTank::QuarantineTank(int id, std::string name, double volumeLiters,
                               WaterParameters water, Location location, int maxAnimals,
                               int daysRequired)
    : Tank(id, std::move(name), volumeLiters, water, std::move(location), maxAnimals),
      daysRequired_(daysRequired) {
    if (daysRequired <= 0) {
        throw InvalidParameterException("quarantine duration must be positive");
    }
    open_ = false;  // карантин закрыт для посетителей
}

void QuarantineTank::admit(std::shared_ptr<Animal> animal, const Date& date) {
    Animal* raw = animal.get();
    addAnimal(std::move(animal));
    raw->health().setQuarantined(true);
    admissions_.insert_or_assign(raw->id(), date);
}

const Date& QuarantineTank::admissionDate(int animalId) const {
    auto it = admissions_.find(animalId);
    if (it == admissions_.end()) {
        throw AnimalNotFoundException(animalId);
    }
    return it->second;
}

bool QuarantineTank::canRelease(int animalId, const Date& today) const {
    return daysRemaining(animalId, today) == 0 &&
           !findAnimal(animalId).health().hasSeriousCondition();
}

int QuarantineTank::daysRemaining(int animalId, const Date& today) const {
    int passed = admissionDate(animalId).daysUntil(today);
    return std::max(0, daysRequired_ - passed);
}

std::shared_ptr<Animal> QuarantineTank::release(int animalId, const Date& today) {
    if (!canRelease(animalId, today)) {
        throw QuarantineViolationException("animal #" + std::to_string(animalId) +
                                           " cannot leave quarantine yet");
    }
    auto animal = removeAnimal(animalId);
    animal->health().setQuarantined(false);
    admissions_.erase(animalId);
    return animal;
}

// ---------------------------------------------------------------- Exhibit

Exhibit::Exhibit(std::string name, std::string theme)
    : name_(std::move(name)), theme_(std::move(theme)), visitorCount_(0) {
    if (name_.empty()) {
        throw InvalidParameterException("exhibit name must be non-empty");
    }
}

void Exhibit::addTank(Tank* tank) {
    if (tank == nullptr) {
        throw InvalidParameterException("tank must not be null");
    }
    for (const Tank* t : tanks_) {
        if (t->id() == tank->id()) {
            throw InvalidParameterException("tank already in exhibit");
        }
    }
    tanks_.push_back(tank);
}

bool Exhibit::removeTank(int tankId) {
    auto it = std::find_if(tanks_.begin(), tanks_.end(),
                           [tankId](const Tank* t) { return t->id() == tankId; });
    if (it == tanks_.end()) {
        return false;
    }
    tanks_.erase(it);
    return true;
}

int Exhibit::totalAnimals() const {
    int total = 0;
    for (const Tank* t : tanks_) {
        total += t->animalCount();
    }
    return total;
}

int Exhibit::speciesDiversity() const {
    std::set<std::string> names;
    for (const Tank* t : tanks_) {
        for (const auto& a : t->animals()) {
            names.insert(a->species().name());
        }
    }
    return static_cast<int>(names.size());
}

bool Exhibit::isOpen() const {
    return std::any_of(tanks_.begin(), tanks_.end(), [](const Tank* t) { return t->isOpen(); });
}

void Exhibit::registerVisit() {
    if (!isOpen()) {
        throw OceanariumException("exhibit " + name_ + " is closed");
    }
    ++visitorCount_;
}

// ---------------------------------------------------------------- EnvironmentMonitor

EnvironmentMonitor::EnvironmentMonitor() : checksPerformed_(0) {}

void EnvironmentMonitor::addSensor(const Sensor& sensor) {
    sensors_.push_back(sensor);
}

int EnvironmentMonitor::check(const Tank& tank) {
    int newAlerts = 0;
    for (auto& sensor : sensors_) {
        sensor.read(tank.water());
        if (sensor.isAlarm()) {
            alerts_.push_back(tank.name() + ": sensor " + sensor.id() + " out of range (" +
                              std::to_string(sensor.lastValue()) + ")");
            ++newAlerts;
        }
    }
    if (tank.needsCleaning()) {
        alerts_.push_back(tank.name() + ": cleaning required");
        ++newAlerts;
    }
    ++checksPerformed_;
    return newAlerts;
}

void EnvironmentMonitor::calibrateAll() {
    for (auto& sensor : sensors_) {
        sensor.calibrate();
    }
}

int EnvironmentMonitor::alertCount() const {
    return static_cast<int>(alerts_.size());
}

void EnvironmentMonitor::clearAlerts() {
    alerts_.clear();
}

}  // namespace ocean
