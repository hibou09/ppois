/**
 * @file Water.h
 * @brief Параметры воды, требования видов к воде и оборудование жизнеобеспечения.
 */
#pragma once

#include <string>

namespace ocean {

/**
 * @brief Текущие параметры воды в аквариуме.
 * @details Хранит температуру, солёность, pH, содержание кислорода и аммиака.
 *          Конструктор и изменяющие методы следят за допустимыми диапазонами значений.
 */
class WaterParameters {
public:
    /**
     * @brief Создаёт набор параметров воды.
     * @param temperature Температура, °C (допустимо от -2 до 40).
     * @param salinity Солёность, ppt (от 0 до 50).
     * @param ph Кислотность pH (от 0 до 14).
     * @param oxygen Содержание кислорода, мг/л (неотрицательное).
     * @param ammonia Содержание аммиака, мг/л (неотрицательное, по умолчанию 0).
     * @throws InvalidWaterParametersException если какое-либо значение вне допустимого диапазона.
     */
    WaterParameters(double temperature, double salinity, double ph, double oxygen,
                    double ammonia = 0.0);

    /// @brief Температура воды, °C.
    double temperature() const { return temperature_; }
    /// @brief Солёность воды, ppt.
    double salinity() const { return salinity_; }
    /// @brief Кислотность воды (pH).
    double ph() const { return ph_; }
    /// @brief Содержание кислорода, мг/л.
    double oxygen() const { return oxygen_; }
    /// @brief Содержание аммиака, мг/л.
    double ammonia() const { return ammonia_; }

    /**
     * @brief Изменяет температуру на заданную величину.
     * @param delta Изменение температуры, °C (может быть отрицательным).
     * @throws InvalidWaterParametersException если новая температура выходит за [-2, 40].
     */
    void adjustTemperature(double delta);
    /**
     * @brief Добавляет аммиак (например, от отходов животных).
     * @param amount Добавляемое количество, мг/л.
     * @throws InvalidParameterException если amount отрицательно.
     */
    void addAmmonia(double amount);
    /**
     * @brief Уменьшает содержание аммиака на долю fraction (0..1).
     * @param fraction Доля, на которую уменьшается аммиак.
     * @throws InvalidParameterException если fraction вне диапазона [0, 1].
     */
    void reduceAmmonia(double fraction);
    /**
     * @brief Подмена воды: доля fraction заменяется водой fresh.
     * @param fresh Параметры свежей воды.
     * @param fraction Доля заменяемой воды (0..1).
     * @throws InvalidParameterException если fraction вне диапазона [0, 1].
     */
    void mixWith(const WaterParameters& fresh, double fraction);
    /**
     * @brief Проверяет, опасна ли вода для обитателей.
     * @return true, если аммиака больше 0.5 или кислорода меньше 3.0.
     */
    bool isToxic() const;
    /**
     * @brief Интегральный индекс качества воды 0..100.
     * @details Штрафуется за аммиак, отклонение pH от 8.1 и нехватку кислорода (меньше 6).
     * @return Индекс качества от 0 до 100.
     */
    double qualityIndex() const;

private:
    /**
     * @brief Проверяет допустимость солёности, pH, кислорода и аммиака.
     * @param salinity Солёность, ppt.
     * @param ph Кислотность.
     * @param oxygen Кислород, мг/л.
     * @param ammonia Аммиак, мг/л.
     * @throws InvalidWaterParametersException если значение вне допустимого диапазона.
     */
    static void validate(double salinity, double ph, double oxygen, double ammonia);

    double temperature_;  ///< Температура, °C.
    double salinity_;     ///< Солёность, ppt.
    double ph_;           ///< Кислотность (pH).
    double oxygen_;       ///< Содержание кислорода, мг/л.
    double ammonia_;      ///< Содержание аммиака, мг/л.
};

/**
 * @brief Диапазоны параметров воды, допустимые для вида.
 */
class WaterRequirements {
public:
    /**
     * @brief Создаёт требования вида к воде.
     * @param minTemp Минимальная температура, °C.
     * @param maxTemp Максимальная температура, °C.
     * @param minSalinity Минимальная солёность, ppt.
     * @param maxSalinity Максимальная солёность, ppt.
     * @param minPh Минимальный pH.
     * @param maxPh Максимальный pH.
     * @param minOxygen Минимальное содержание кислорода, мг/л.
     * @throws InvalidParameterException если минимум больше максимума или кислород отрицателен.
     */
    WaterRequirements(double minTemp, double maxTemp, double minSalinity, double maxSalinity,
                      double minPh, double maxPh, double minOxygen);

    /// @brief Минимальная допустимая температура, °C.
    double minTemp() const { return minTemp_; }
    /// @brief Максимальная допустимая температура, °C.
    double maxTemp() const { return maxTemp_; }
    /// @brief Минимальная допустимая солёность, ppt.
    double minSalinity() const { return minSalinity_; }
    /// @brief Максимальная допустимая солёность, ppt.
    double maxSalinity() const { return maxSalinity_; }
    /// @brief Минимальный допустимый pH.
    double minPh() const { return minPh_; }
    /// @brief Максимальный допустимый pH.
    double maxPh() const { return maxPh_; }
    /// @brief Минимально необходимое содержание кислорода, мг/л.
    double minOxygen() const { return minOxygen_; }

    /**
     * @brief Проверяет, подходит ли вода виду.
     * @param water Проверяемые параметры воды.
     * @return true, если все параметры в диапазонах, кислорода достаточно и вода не токсична.
     */
    bool accepts(const WaterParameters& water) const;
    /**
     * @brief Определяет, нужна ли виду морская вода.
     * @return true, если минимальная солёность не меньше 20 ppt.
     */
    bool isSaltwater() const;
    /**
     * @brief Проверяет, пересекаются ли диапазоны температуры, солёности и pH.
     * @param other Требования другого вида.
     * @return true, если у видов есть общие допустимые условия.
     */
    bool overlaps(const WaterRequirements& other) const;
    /**
     * @brief Пересечение диапазонов двух видов.
     * @param other Требования другого вида.
     * @return Требования, подходящие обоим видам.
     * @throws IncompatibleSpeciesException если диапазоны не пересекаются.
     */
    WaterRequirements intersect(const WaterRequirements& other) const;
    /**
     * @brief Идеальные параметры — середины диапазонов.
     * @return Параметры воды; кислород берётся на 1 мг/л выше минимума, аммиак равен 0.
     */
    WaterParameters ideal() const;
    /**
     * @brief Суммарное отклонение параметров от допустимых диапазонов.
     * @param water Проверяемые параметры воды.
     * @return Сумма отклонений по температуре, солёности, pH и кислороду (0, если всё в норме).
     */
    double deviation(const WaterParameters& water) const;

private:
    double minTemp_;      ///< Минимальная температура, °C.
    double maxTemp_;      ///< Максимальная температура, °C.
    double minSalinity_;  ///< Минимальная солёность, ppt.
    double maxSalinity_;  ///< Максимальная солёность, ppt.
    double minPh_;        ///< Минимальный pH.
    double maxPh_;        ///< Максимальный pH.
    double minOxygen_;    ///< Минимальное содержание кислорода, мг/л.
};

/**
 * @brief Тип измеряемой величины датчика.
 */
enum class SensorType {
    Temperature,  ///< Температура воды.
    Salinity,     ///< Солёность.
    Ph,           ///< Кислотность.
    Oxygen,       ///< Содержание кислорода.
    Ammonia       ///< Содержание аммиака.
};

/**
 * @brief Датчик параметра воды.
 * @details После kDriftLimit измерений датчик теряет калибровку и требует calibrate().
 */
class Sensor {
public:
    /**
     * @brief Создаёт датчик.
     * @param id Идентификатор датчика (непустой).
     * @param type Измеряемая величина.
     * @param minThreshold Нижняя граница нормы.
     * @param maxThreshold Верхняя граница нормы.
     * @param calibrated Откалиброван ли датчик изначально.
     * @throws InvalidParameterException если id пуст или нижняя граница больше верхней.
     */
    Sensor(std::string id, SensorType type, double minThreshold, double maxThreshold,
           bool calibrated = true);

    /// @brief Идентификатор датчика.
    const std::string& id() const { return id_; }
    /// @brief Тип измеряемой величины.
    SensorType type() const { return type_; }
    /// @brief Последнее снятое показание.
    double lastValue() const { return lastValue_; }
    /// @brief Откалиброван ли датчик в данный момент.
    bool isCalibrated() const { return calibrated_; }

    /**
     * @brief Снимает показание; после kDriftLimit измерений датчик теряет калибровку.
     * @param water Параметры воды, с которых снимается показание.
     * @return Значение измеряемой величины.
     * @throws SensorNotCalibratedException если датчик не откалиброван.
     */
    double read(const WaterParameters& water);
    /**
     * @brief Проверяет, вышло ли последнее показание за пороги.
     * @return true, если показание вне диапазона [minThreshold, maxThreshold].
     */
    bool isAlarm() const;
    /// @brief Калибрует датчик и обнуляет счётчик измерений.
    void calibrate();

    /// Число измерений, после которого датчик теряет калибровку.
    static const int kDriftLimit = 50;

private:
    std::string id_;                  ///< Идентификатор датчика.
    SensorType type_;                 ///< Измеряемая величина.
    double minThreshold_;             ///< Нижний порог нормы.
    double maxThreshold_;             ///< Верхний порог нормы.
    double lastValue_;                ///< Последнее показание.
    bool calibrated_;                 ///< Признак калибровки.
    int readingsSinceCalibration_;    ///< Число измерений с последней калибровки.
};

/**
 * @brief Система фильтрации воды.
 * @details Очищает воду от аммиака; фильтр изнашивается и требует замены.
 */
class FiltrationSystem {
public:
    /**
     * @brief Создаёт систему фильтрации.
     * @param model Название модели.
     * @param flowRateLph Производительность, л/ч.
     * @throws InvalidParameterException если производительность не положительна.
     */
    FiltrationSystem(std::string model, double flowRateLph);

    /// @brief Название модели.
    const std::string& model() const { return model_; }
    /// @brief Износ фильтра, % (0..100).
    double filterWear() const { return filterWear_; }
    /// @brief Включена ли система.
    bool isActive() const { return active_; }
    /// @brief Включает или выключает систему.
    void setActive(bool active) { active_ = active; }

    /**
     * @brief Запускает цикл фильтрации: снижает аммиак и увеличивает износ фильтра.
     * @param water Очищаемая вода.
     * @param hours Длительность цикла, ч.
     * @throws InvalidParameterException если hours не положительно.
     * @throws EquipmentFailureException если система выключена или фильтр засорён (износ 100%).
     */
    void runCycle(WaterParameters& water, double hours);
    /// @brief Заменяет фильтр (износ становится 0).
    void replaceFilter();
    /**
     * @brief Проверяет, нужно ли обслуживание.
     * @return true, если износ фильтра не меньше 80%.
     */
    bool needsMaintenance() const;
    /**
     * @brief Сколько раз в час система прогоняет весь объём аквариума.
     * @param volumeLiters Объём аквариума, л.
     * @return Кратность оборота воды, 1/ч.
     * @throws InvalidParameterException если объём не положителен.
     */
    double turnoverPerHour(double volumeLiters) const;

private:
    std::string model_;    ///< Название модели.
    double flowRateLph_;   ///< Производительность, л/ч.
    double filterWear_;    ///< Износ фильтра, %.
    bool active_;          ///< Включена ли система.
};

/**
 * @brief Нагреватель/охладитель воды.
 */
class HeatingSystem {
public:
    /**
     * @brief Создаёт систему терморегуляции.
     * @param powerKw Мощность, кВт.
     * @param targetTemp Целевая температура, °C (от 0 до 35).
     * @throws InvalidParameterException если мощность не положительна или температура вне [0, 35].
     */
    HeatingSystem(double powerKw, double targetTemp);

    /// @brief Целевая температура, °C.
    double targetTemperature() const { return targetTemp_; }
    /// @brief Включена ли система.
    bool isActive() const { return active_; }
    /// @brief Включает или выключает систему.
    void setActive(bool active) { active_ = active; }

    /**
     * @brief Задаёт целевую температуру.
     * @param temp Новая целевая температура, °C.
     * @throws InvalidParameterException если temp вне диапазона [0, 35].
     */
    void setTargetTemperature(double temp);
    /**
     * @brief Приближает температуру воды к целевой за hours часов.
     * @details Если система выключена или hours не положительно, вода не меняется.
     * @param water Регулируемая вода.
     * @param hours Время работы, ч.
     * @throws InvalidWaterParametersException если температура воды выйдет за допустимые пределы.
     */
    void regulate(WaterParameters& water, double hours) const;
    /**
     * @brief Потребление энергии за период.
     * @param hours Время работы, ч.
     * @return Энергия, кВт·ч (0, если система выключена).
     */
    double energyConsumption(double hours) const;

private:
    double powerKw_;    ///< Мощность, кВт.
    double targetTemp_; ///< Целевая температура, °C.
    bool active_;       ///< Включена ли система.
};

/**
 * @brief Освещение аквариума (свет включается в 08:00).
 */
class LightingSystem {
public:
    /**
     * @brief Создаёт систему освещения.
     * @param powerW Мощность, Вт.
     * @param dayLengthHours Длина светового дня, ч (от 0 до 16).
     * @param intensityLux Интенсивность света, лк.
     * @throws InvalidParameterException если мощность не положительна, интенсивность
     *         отрицательна или длина дня вне [0, 16].
     */
    LightingSystem(double powerW, double dayLengthHours, double intensityLux);

    /// @brief Длина светового дня, ч.
    double dayLength() const { return dayLengthHours_; }
    /// @brief Интенсивность света, лк.
    double intensity() const { return intensityLux_; }
    /// @brief Включена ли система.
    bool isActive() const { return active_; }
    /// @brief Включает или выключает систему.
    void setActive(bool active) { active_ = active; }

    /**
     * @brief Задаёт длину светового дня.
     * @param hours Длина дня, ч.
     * @throws InvalidParameterException если hours вне диапазона [0, 16].
     */
    void setDayLength(double hours);
    /**
     * @brief Горит ли свет в указанную минуту суток.
     * @param minuteOfDay Минута суток (0..1439).
     * @return true, если система включена и время в пределах светового дня (с 08:00).
     */
    bool isLightOn(int minuteOfDay) const;
    /**
     * @brief Суточное потребление энергии.
     * @return Энергия, кВт·ч (0, если система выключена).
     */
    double dailyEnergyKwh() const;
    /**
     * @brief Уменьшает яркость света.
     * @param percent На сколько процентов снизить интенсивность (0..100).
     * @throws InvalidParameterException если percent вне диапазона [0, 100].
     */
    void dim(double percent);

private:
    double powerW_;          ///< Мощность, Вт.
    double dayLengthHours_;  ///< Длина светового дня, ч.
    double intensityLux_;    ///< Интенсивность света, лк.
    bool active_;            ///< Включена ли система.
};

}  // namespace ocean
