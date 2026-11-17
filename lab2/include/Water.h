/**
 * @file Water.h
 * @brief Параметры воды, требования видов к воде и оборудование жизнеобеспечения.
 */
#pragma once

#include <string>

namespace ocean {

/// Текущие параметры воды в аквариуме.
class WaterParameters {
public:
    WaterParameters(double temperature, double salinity, double ph, double oxygen,
                    double ammonia = 0.0);

    double temperature() const { return temperature_; }
    double salinity() const { return salinity_; }
    double ph() const { return ph_; }
    double oxygen() const { return oxygen_; }
    double ammonia() const { return ammonia_; }

    void adjustTemperature(double delta);
    void addAmmonia(double amount);
    /// Уменьшить содержание аммиака на долю fraction (0..1).
    void reduceAmmonia(double fraction);
    /// Подмена воды: доля fraction заменяется водой fresh.
    void mixWith(const WaterParameters& fresh, double fraction);
    bool isToxic() const;
    /// Интегральный индекс качества воды 0..100.
    double qualityIndex() const;

private:
    static void validate(double salinity, double ph, double oxygen, double ammonia);

    double temperature_;
    double salinity_;
    double ph_;
    double oxygen_;
    double ammonia_;
};

/// Диапазоны параметров воды, допустимые для вида.
class WaterRequirements {
public:
    WaterRequirements(double minTemp, double maxTemp, double minSalinity, double maxSalinity,
                      double minPh, double maxPh, double minOxygen);

    double minTemp() const { return minTemp_; }
    double maxTemp() const { return maxTemp_; }
    double minSalinity() const { return minSalinity_; }
    double maxSalinity() const { return maxSalinity_; }
    double minPh() const { return minPh_; }
    double maxPh() const { return maxPh_; }
    double minOxygen() const { return minOxygen_; }

    bool accepts(const WaterParameters& water) const;
    bool isSaltwater() const;
    bool overlaps(const WaterRequirements& other) const;
    /// Пересечение диапазонов; бросает IncompatibleSpeciesException, если оно пусто.
    WaterRequirements intersect(const WaterRequirements& other) const;
    /// Идеальные параметры — середины диапазонов.
    WaterParameters ideal() const;
    /// Суммарное отклонение параметров от допустимых диапазонов.
    double deviation(const WaterParameters& water) const;

private:
    double minTemp_;
    double maxTemp_;
    double minSalinity_;
    double maxSalinity_;
    double minPh_;
    double maxPh_;
    double minOxygen_;
};

/// Тип измеряемой величины датчика.
enum class SensorType { Temperature, Salinity, Ph, Oxygen, Ammonia };

/// Датчик параметра воды.
class Sensor {
public:
    Sensor(std::string id, SensorType type, double minThreshold, double maxThreshold,
           bool calibrated = true);

    const std::string& id() const { return id_; }
    SensorType type() const { return type_; }
    double lastValue() const { return lastValue_; }
    bool isCalibrated() const { return calibrated_; }

    /// Снять показание; после kDriftLimit измерений датчик теряет калибровку.
    double read(const WaterParameters& water);
    bool isAlarm() const;
    void calibrate();

    static const int kDriftLimit = 50;

private:
    std::string id_;
    SensorType type_;
    double minThreshold_;
    double maxThreshold_;
    double lastValue_;
    bool calibrated_;
    int readingsSinceCalibration_;
};

/// Система фильтрации воды.
class FiltrationSystem {
public:
    FiltrationSystem(std::string model, double flowRateLph);

    const std::string& model() const { return model_; }
    double filterWear() const { return filterWear_; }
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

    void runCycle(WaterParameters& water, double hours);
    void replaceFilter();
    bool needsMaintenance() const;
    double turnoverPerHour(double volumeLiters) const;

private:
    std::string model_;
    double flowRateLph_;
    double filterWear_;
    bool active_;
};

/// Нагреватель/охладитель воды.
class HeatingSystem {
public:
    HeatingSystem(double powerKw, double targetTemp);

    double targetTemperature() const { return targetTemp_; }
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

    void setTargetTemperature(double temp);
    /// Приблизить температуру воды к целевой за hours часов.
    void regulate(WaterParameters& water, double hours) const;
    double energyConsumption(double hours) const;

private:
    double powerKw_;
    double targetTemp_;
    bool active_;
};

/// Освещение аквариума (свет включается в 08:00).
class LightingSystem {
public:
    LightingSystem(double powerW, double dayLengthHours, double intensityLux);

    double dayLength() const { return dayLengthHours_; }
    double intensity() const { return intensityLux_; }
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

    void setDayLength(double hours);
    bool isLightOn(int minuteOfDay) const;
    double dailyEnergyKwh() const;
    void dim(double percent);

private:
    double powerW_;
    double dayLengthHours_;
    double intensityLux_;
    bool active_;
};

}  // namespace ocean
