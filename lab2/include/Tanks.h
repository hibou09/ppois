/**
 * @file Tanks.h
 * @brief Аквариумы, экспозиции и мониторинг среды.
 */
#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Animals.h"
#include "Common.h"
#include "Water.h"

namespace ocean {

/// Базовый аквариум.
class Tank {
public:
    Tank(int id, std::string name, double volumeLiters, WaterParameters water, Location location,
         int maxAnimals);
    virtual ~Tank() = default;

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    double volumeLiters() const { return volumeLiters_; }
    WaterParameters& water() { return water_; }
    const WaterParameters& water() const { return water_; }
    const Location& location() const { return location_; }
    int maxAnimals() const { return maxAnimals_; }
    bool isOpen() const { return open_; }
    void setOpen(bool open) { open_ = open; }
    FiltrationSystem& filtration() { return filtration_; }
    HeatingSystem& heating() { return heating_; }
    const std::vector<std::shared_ptr<Animal>>& animals() const { return animals_; }

    virtual std::string type() const { return "Tank"; }
    /// Ограничения конкретного типа аквариума на обитателей.
    virtual bool canHost(const Animal& animal) const;

    /// Заселить животное; проверяет вместимость, воду и совместимость.
    void addAnimal(std::shared_ptr<Animal> animal);
    std::shared_ptr<Animal> removeAnimal(int animalId);
    Animal& findAnimal(int animalId) const;
    bool hasAnimal(int animalId) const;
    int animalCount() const;
    double totalBiomass() const;
    /// Биомасса, кг на 1000 литров.
    double stockingDensity() const;
    double dailyFoodRequirement() const;
    int speciesCount() const;
    int hungryAnimals() const;
    void performWaterChange(double fraction);
    /// Работа систем жизнеобеспечения в течение hours часов.
    void runLifeSupport(double hours);
    bool needsCleaning() const;
    bool isHealthy() const;

protected:
    int id_;
    std::string name_;
    double volumeLiters_;
    WaterParameters water_;
    Location location_;
    int maxAnimals_;
    bool open_;
    std::vector<std::shared_ptr<Animal>> animals_;
    FiltrationSystem filtration_;
    HeatingSystem heating_;
};

/// Рифовый аквариум с кораллами.
class CoralReefTank : public Tank {
public:
    CoralReefTank(int id, std::string name, double volumeLiters, WaterParameters water,
                  Location location, int maxAnimals, double waveIntensity);

    const std::vector<Coral>& corals() const { return corals_; }
    double waveIntensity() const { return waveIntensity_; }

    std::string type() const override { return "CoralReefTank"; }
    bool canHost(const Animal& animal) const override;
    void addCoral(const Coral& coral);
    double totalCoralArea() const;
    void growCorals();
    int bleachedCount() const;
    bool bleachingRisk() const;

private:
    std::vector<Coral> corals_;
    double waveIntensity_;
};

/// Большой аквариум «Открытый океан» с подводным тоннелем.
class OpenOceanTank : public Tank {
public:
    OpenOceanTank(int id, std::string name, double volumeLiters, WaterParameters water,
                  Location location, int maxAnimals, double depthM, double tunnelLengthM);

    double depth() const { return depthM_; }
    double tunnelLength() const { return tunnelLengthM_; }

    std::string type() const override { return "OpenOceanTank"; }
    bool hasTunnel() const;
    int tunnelVisitorCapacity() const;
    bool requiresDiverCertification() const;
    int predatorCount() const;

private:
    double depthM_;
    double tunnelLengthM_;
};

/// Контактный бассейн, где посетители могут трогать животных.
class TouchPool : public Tank {
public:
    TouchPool(int id, std::string name, double volumeLiters, WaterParameters water,
              Location location, int maxAnimals, int maxTouchesPerDay);

    int touchesToday() const { return touchesToday_; }

    std::string type() const override { return "TouchPool"; }
    bool canHost(const Animal& animal) const override;
    bool allowTouch();
    int remainingTouches() const;
    void resetDay();

private:
    int maxTouchesPerDay_;
    int touchesToday_;
};

/// Карантинный аквариум для новых и больных животных.
class QuarantineTank : public Tank {
public:
    QuarantineTank(int id, std::string name, double volumeLiters, WaterParameters water,
                   Location location, int maxAnimals, int daysRequired);

    int daysRequired() const { return daysRequired_; }

    std::string type() const override { return "QuarantineTank"; }
    void admit(std::shared_ptr<Animal> animal, const Date& date);
    bool canRelease(int animalId, const Date& today) const;
    int daysRemaining(int animalId, const Date& today) const;
    /// Выпустить животное; бросает QuarantineViolationException при досрочном выпуске.
    std::shared_ptr<Animal> release(int animalId, const Date& today);

private:
    const Date& admissionDate(int animalId) const;

    int daysRequired_;
    std::map<int, Date> admissions_;
};

/// Тематическая экспозиция из нескольких аквариумов.
class Exhibit {
public:
    Exhibit(std::string name, std::string theme);

    const std::string& name() const { return name_; }
    const std::string& theme() const { return theme_; }
    int visitorCount() const { return visitorCount_; }
    const std::vector<Tank*>& tanks() const { return tanks_; }

    void addTank(Tank* tank);
    bool removeTank(int tankId);
    int totalAnimals() const;
    int speciesDiversity() const;
    bool isOpen() const;
    void registerVisit();

private:
    std::string name_;
    std::string theme_;
    std::vector<Tank*> tanks_;
    int visitorCount_;
};

/// Мониторинг среды: набор датчиков и журнал тревог.
class EnvironmentMonitor {
public:
    EnvironmentMonitor();

    const std::vector<std::string>& alerts() const { return alerts_; }
    int checksPerformed() const { return checksPerformed_; }
    std::vector<Sensor>& sensors() { return sensors_; }

    void addSensor(const Sensor& sensor);
    /// Опросить датчики по воде аквариума; вернуть число новых тревог.
    int check(const Tank& tank);
    void calibrateAll();
    int alertCount() const;
    void clearAlerts();

private:
    std::vector<Sensor> sensors_;
    std::vector<std::string> alerts_;
    int checksPerformed_;
};

}  // namespace ocean
