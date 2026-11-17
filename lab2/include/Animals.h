/**
 * @file Animals.h
 * @brief Виды и обитатели океанариума.
 */
#pragma once

#include <string>
#include <vector>

#include "Common.h"
#include "Food.h"
#include "Health.h"
#include "Water.h"

namespace ocean {

enum class Diet { Herbivore, Carnivore, Omnivore, Planktivore };

enum class ConservationStatus {
    LeastConcern,
    NearThreatened,
    Vulnerable,
    Endangered,
    CriticallyEndangered
};

/// Биологический вид.
class Species {
public:
    Species(std::string name, std::string latinName, Diet diet, WaterRequirements requirements,
            bool predatory, ConservationStatus status);

    const std::string& name() const { return name_; }
    const std::string& latinName() const { return latinName_; }
    Diet diet() const { return diet_; }
    const WaterRequirements& requirements() const { return requirements_; }
    bool isPredatory() const { return predatory_; }
    ConservationStatus status() const { return status_; }

    bool isEndangered() const;
    /// Можно ли держать два вида в одном аквариуме.
    bool canCoexistWith(const Species& other) const;

private:
    std::string name_;
    std::string latinName_;
    Diet diet_;
    WaterRequirements requirements_;
    bool predatory_;
    ConservationStatus status_;
};

/// Абстрактный обитатель океанариума.
class Animal {
public:
    Animal(int id, std::string name, Species species, int ageYears, double weightKg);
    virtual ~Animal() = default;

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    const Species& species() const { return species_; }
    int ageYears() const { return ageYears_; }
    double weightKg() const { return weightKg_; }
    double hunger() const { return hunger_; }
    HealthRecord& health() { return health_; }
    const HealthRecord& health() const { return health_; }

    virtual double dailyFoodKg() const = 0;
    virtual std::string kind() const = 0;
    virtual std::string describe() const;
    /// Прошли сутки: растёт голод.
    virtual void passDay();

    /// Покормить животное kg килограммами корма food.
    void feed(FoodItem& food, double kg, const Date& today);
    bool isHungry() const;
    void celebrateBirthday();
    bool canLiveIn(const WaterParameters& water) const;
    bool canShareTankWith(const Animal& other) const;

protected:
    void increaseHunger(double amount);

    int id_;
    std::string name_;
    Species species_;
    int ageYears_;
    double weightKg_;
    double hunger_;
    HealthRecord health_;
};

/// Рыба.
class Fish : public Animal {
public:
    Fish(int id, std::string name, Species species, int ageYears, double weightKg,
         double lengthCm, bool schooling);

    double lengthCm() const { return lengthCm_; }
    bool isSchooling() const { return schooling_; }

    double dailyFoodKg() const override;
    std::string kind() const override { return "Fish"; }
    /// Крейсерская скорость, м/с.
    double swimSpeed() const;
    void grow(double cm);

protected:
    double lengthCm_;
    bool schooling_;
};

/// Акула.
class Shark : public Fish {
public:
    Shark(int id, std::string name, Species species, int ageYears, double weightKg,
          double lengthCm, double biteForceN);

    double biteForce() const { return biteForceN_; }
    int teethLost() const { return teethLost_; }

    double dailyFoodKg() const override;
    std::string kind() const override { return "Shark"; }
    bool isDangerousTo(const Animal& other) const;
    void loseTeeth(int count);

private:
    double biteForceN_;
    int teethLost_;
};

/// Скат.
class Ray : public Fish {
public:
    Ray(int id, std::string name, Species species, int ageYears, double weightKg,
        double wingspanCm, bool venomous);

    double wingspan() const { return wingspanCm_; }
    bool isVenomous() const { return venomous_; }

    std::string kind() const override { return "Ray"; }
    bool isSafeForTouch() const;
    void clipBarb();

private:
    double wingspanCm_;
    bool venomous_;
    bool barbClipped_;
};

/// Морская черепаха.
class Turtle : public Animal {
public:
    Turtle(int id, std::string name, Species species, int ageYears, double weightKg,
           double shellLengthCm);

    double shellLength() const { return shellLengthCm_; }
    double baskingHoursToday() const { return baskingHoursToday_; }

    double dailyFoodKg() const override;
    std::string kind() const override { return "Turtle"; }
    void passDay() override;
    void bask(double hours);
    bool needsBasking() const;
    int remainingLifespan() const;

    static const int kLifespanYears = 80;

private:
    double shellLengthCm_;
    double baskingHoursToday_;
};

/// Осьминог.
class Octopus : public Animal {
public:
    Octopus(int id, std::string name, Species species, int ageYears, double weightKg,
            int intelligence);

    int armCount() const { return armCount_; }
    int intelligence() const { return intelligence_; }
    int escapeAttempts() const { return escapeAttempts_; }

    double dailyFoodKg() const override;
    std::string kind() const override { return "Octopus"; }
    /// Решить головоломку сложности 1..10 (элемент обогащения среды).
    bool solvePuzzle(int difficulty);
    /// Попытка побега; успешна, если крышка не закреплена.
    bool attemptEscape(bool lidSecured);
    void loseArm();
    void regenerateArm();

private:
    int armCount_;
    int intelligence_;
    int escapeAttempts_;
};

/// Медуза.
class Jellyfish : public Animal {
public:
    Jellyfish(int id, std::string name, Species species, int ageYears, double weightKg,
              double bellDiameterCm, bool stinging);

    double bellDiameter() const { return bellDiameterCm_; }
    bool isStinging() const { return stinging_; }

    double dailyFoodKg() const override;
    std::string kind() const override { return "Jellyfish"; }
    /// Частота пульсаций купола (в минуту) при заданной температуре.
    double pulseRate(double waterTemperature) const;
    bool isSafeForTouch() const;

private:
    double bellDiameterCm_;
    bool stinging_;
};

/// Пингвин.
class Penguin : public Animal {
public:
    Penguin(int id, std::string name, Species species, int ageYears, double weightKg,
            std::string colony);

    const std::string& colony() const { return colony_; }
    bool isMolting() const { return molting_; }
    double swimHoursToday() const { return swimHoursToday_; }

    double dailyFoodKg() const override;
    std::string kind() const override { return "Penguin"; }
    void passDay() override;
    void swim(double hours);
    void startMolt();
    void finishMolt();
    bool hadEnoughExercise() const;

private:
    std::string colony_;
    double swimHoursToday_;
    bool molting_;
};

/// Тюлень — участник шоу.
class Seal : public Animal {
public:
    Seal(int id, std::string name, Species species, int ageYears, double weightKg,
         double blubberCm);

    const std::vector<std::string>& tricks() const { return tricks_; }
    int performances() const { return performances_; }

    double dailyFoodKg() const override;
    std::string kind() const override { return "Seal"; }
    bool learnTrick(const std::string& trick);
    bool knowsTrick(const std::string& trick) const;
    bool perform(const std::string& trick);

private:
    std::vector<std::string> tricks_;
    double blubberCm_;
    int performances_;
};

/// Колония кораллов.
class Coral {
public:
    Coral(std::string speciesName, double sizeCm2, double healthPercent = 100.0);

    const std::string& speciesName() const { return speciesName_; }
    double size() const { return sizeCm2_; }
    double healthPercent() const { return healthPercent_; }
    bool isBleached() const { return bleached_; }

    /// Рост за сутки при данных параметрах воды; возвращает новую площадь.
    double grow(const WaterParameters& water);
    /// Отделить фрагмент площадью cm2 для размножения.
    Coral fragment(double cm2);
    void recover();

    static constexpr double kBleachingTemperature = 29.0;

private:
    std::string speciesName_;
    double sizeCm2_;
    double healthPercent_;
    bool bleached_;
};

}  // namespace ocean
