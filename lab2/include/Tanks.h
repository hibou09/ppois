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

/**
 * @brief Базовый аквариум.
 * @details Хранит обитателей, воду и системы жизнеобеспечения (фильтрация, нагрев).
 * Наследники уточняют правила заселения через виртуальный метод canHost().
 */
class Tank {
public:
    /**
     * @brief Создаёт аквариум.
     * @param id Положительный идентификатор.
     * @param name Непустое название.
     * @param volumeLiters Объём в литрах (> 0).
     * @param water Начальные параметры воды.
     * @param location Расположение в океанариуме.
     * @param maxAnimals Максимальное число обитателей (> 0).
     * @throws InvalidParameterException если id, объём или вместимость не положительны либо имя пустое.
     */
    Tank(int id, std::string name, double volumeLiters, WaterParameters water, Location location,
         int maxAnimals);
    /// @brief Виртуальный деструктор для безопасного удаления через указатель на базовый класс.
    virtual ~Tank() = default;

    /// @brief Идентификатор аквариума.
    int id() const { return id_; }
    /// @brief Название аквариума.
    const std::string& name() const { return name_; }
    /// @brief Объём в литрах.
    double volumeLiters() const { return volumeLiters_; }
    /// @brief Параметры воды (изменяемая ссылка).
    WaterParameters& water() { return water_; }
    /// @brief Параметры воды (только чтение).
    const WaterParameters& water() const { return water_; }
    /// @brief Расположение аквариума.
    const Location& location() const { return location_; }
    /// @brief Максимальное число обитателей.
    int maxAnimals() const { return maxAnimals_; }
    /// @brief Открыт ли аквариум для посетителей.
    bool isOpen() const { return open_; }
    /// @brief Открывает или закрывает аквариум для посетителей.
    /// @param open true — открыть, false — закрыть.
    void setOpen(bool open) { open_ = open; }
    /// @brief Система фильтрации аквариума.
    FiltrationSystem& filtration() { return filtration_; }
    /// @brief Система нагрева аквариума.
    HeatingSystem& heating() { return heating_; }
    /// @brief Список обитателей.
    const std::vector<std::shared_ptr<Animal>>& animals() const { return animals_; }

    /// @brief Название типа аквариума (переопределяется в наследниках).
    virtual std::string type() const { return "Tank"; }
    /**
     * @brief Ограничения конкретного типа аквариума на обитателей.
     * @param animal Проверяемое животное.
     * @return true, если животное может жить в воде этого аквариума (и проходит
     * дополнительные ограничения наследника).
     */
    virtual bool canHost(const Animal& animal) const;

    /**
     * @brief Заселяет животное; проверяет вместимость, воду и совместимость.
     * @param animal Заселяемое животное.
     * @throws InvalidParameterException если указатель пуст или животное уже живёт в аквариуме.
     * @throws TankCapacityExceededException если достигнут лимит обитателей.
     * @throws InvalidWaterParametersException если вода не подходит животному.
     * @throws IncompatibleSpeciesException если тип аквариума не допускает животное или оно
     * несовместимо с кем-то из соседей.
     */
    void addAnimal(std::shared_ptr<Animal> animal);
    /**
     * @brief Убирает животное из аквариума.
     * @param animalId Идентификатор животного.
     * @return Извлечённое животное.
     * @throws AnimalNotFoundException если такого животного в аквариуме нет.
     */
    std::shared_ptr<Animal> removeAnimal(int animalId);
    /**
     * @brief Ищет животное по идентификатору.
     * @param animalId Идентификатор животного.
     * @return Ссылка на найденное животное.
     * @throws AnimalNotFoundException если такого животного в аквариуме нет.
     */
    Animal& findAnimal(int animalId) const;
    /// @brief Проверяет, живёт ли животное с данным идентификатором в аквариуме.
    /// @param animalId Идентификатор животного.
    bool hasAnimal(int animalId) const;
    /// @brief Число обитателей.
    int animalCount() const;
    /// @brief Суммарная масса обитателей, кг.
    double totalBiomass() const;
    /// @brief Биомасса, кг на 1000 литров.
    double stockingDensity() const;
    /// @brief Суммарная суточная потребность в корме, кг.
    double dailyFoodRequirement() const;
    /// @brief Число различных видов среди обитателей.
    int speciesCount() const;
    /// @brief Число голодных животных.
    int hungryAnimals() const;
    /**
     * @brief Частичная замена воды на свежую (с нулевым аммиаком).
     * @param fraction Доля заменяемой воды, (0; 0.5].
     * @throws InvalidParameterException если доля вне допустимого диапазона.
     */
    void performWaterChange(double fraction);
    /**
     * @brief Работа систем жизнеобеспечения в течение hours часов.
     * @details Животные выделяют аммиак, затем работают нагреватель и фильтр.
     * @param hours Длительность работы в часах (> 0).
     * @throws InvalidParameterException если hours не положительно.
     */
    void runLifeSupport(double hours);
    /// @brief Нужна ли чистка (аммиак не ниже 0.2).
    bool needsCleaning() const;
    /// @brief true, если вода подходит всем обитателям.
    bool isHealthy() const;

protected:
    int id_;                                         ///< Идентификатор.
    std::string name_;                               ///< Название.
    double volumeLiters_;                            ///< Объём, л.
    WaterParameters water_;                          ///< Текущие параметры воды.
    Location location_;                              ///< Расположение.
    int maxAnimals_;                                 ///< Максимум обитателей.
    bool open_;                                      ///< Открыт ли для посетителей.
    std::vector<std::shared_ptr<Animal>> animals_;   ///< Обитатели.
    FiltrationSystem filtration_;                    ///< Система фильтрации.
    HeatingSystem heating_;                          ///< Система нагрева.
};

/**
 * @brief Рифовый аквариум с кораллами.
 * @details Не допускает крупных акул (тяжелее 30 кг), которые разрушают риф.
 */
class CoralReefTank : public Tank {
public:
    /**
     * @brief Создаёт рифовый аквариум.
     * @param id Идентификатор.
     * @param name Название.
     * @param volumeLiters Объём, л.
     * @param water Параметры воды.
     * @param location Расположение.
     * @param maxAnimals Максимум обитателей.
     * @param waveIntensity Интенсивность волнения, [0; 10].
     * @throws InvalidParameterException при некорректных базовых параметрах или волнении вне [0; 10].
     */
    CoralReefTank(int id, std::string name, double volumeLiters, WaterParameters water,
                  Location location, int maxAnimals, double waveIntensity);

    /// @brief Список кораллов.
    const std::vector<Coral>& corals() const { return corals_; }
    /// @brief Интенсивность волнения.
    double waveIntensity() const { return waveIntensity_; }

    /// @brief Название типа аквариума.
    std::string type() const override { return "CoralReefTank"; }
    /// @brief Запрещает акул тяжелее 30 кг, для остальных действуют базовые правила.
    /// @param animal Проверяемое животное.
    bool canHost(const Animal& animal) const override;
    /// @brief Добавляет коралл в аквариум.
    /// @param coral Добавляемый коралл.
    void addCoral(const Coral& coral);
    /// @brief Суммарная площадь кораллов.
    double totalCoralArea() const;
    /// @brief Один цикл роста всех кораллов в текущей воде.
    void growCorals();
    /// @brief Число обесцвеченных кораллов.
    int bleachedCount() const;
    /// @brief true, если температура воды близка к порогу обесцвечивания кораллов.
    bool bleachingRisk() const;

private:
    std::vector<Coral> corals_;  ///< Кораллы аквариума.
    double waveIntensity_;       ///< Интенсивность волнения.
};

/// @brief Большой аквариум «Открытый океан» с подводным тоннелем.
class OpenOceanTank : public Tank {
public:
    /**
     * @brief Создаёт аквариум «Открытый океан».
     * @param id Идентификатор.
     * @param name Название.
     * @param volumeLiters Объём, л.
     * @param water Параметры воды.
     * @param location Расположение.
     * @param maxAnimals Максимум обитателей.
     * @param depthM Глубина, м (> 0).
     * @param tunnelLengthM Длина тоннеля, м (>= 0).
     * @throws InvalidParameterException при некорректных базовых параметрах, неположительной
     * глубине или отрицательной длине тоннеля.
     */
    OpenOceanTank(int id, std::string name, double volumeLiters, WaterParameters water,
                  Location location, int maxAnimals, double depthM, double tunnelLengthM);

    /// @brief Глубина, м.
    double depth() const { return depthM_; }
    /// @brief Длина тоннеля, м.
    double tunnelLength() const { return tunnelLengthM_; }

    /// @brief Название типа аквариума.
    std::string type() const override { return "OpenOceanTank"; }
    /// @brief Есть ли подводный тоннель.
    bool hasTunnel() const;
    /// @brief Вместимость тоннеля по числу посетителей (3 человека на метр).
    int tunnelVisitorCapacity() const;
    /// @brief true, если глубина больше 5 м и для работы нужен сертифицированный водолаз.
    bool requiresDiverCertification() const;
    /// @brief Число хищников среди обитателей.
    int predatorCount() const;

private:
    double depthM_;         ///< Глубина, м.
    double tunnelLengthM_;  ///< Длина тоннеля, м.
};

/// @brief Контактный бассейн, где посетители могут трогать животных.
class TouchPool : public Tank {
public:
    /**
     * @brief Создаёт контактный бассейн.
     * @param id Идентификатор.
     * @param name Название.
     * @param volumeLiters Объём, л.
     * @param water Параметры воды.
     * @param location Расположение.
     * @param maxAnimals Максимум обитателей.
     * @param maxTouchesPerDay Лимит прикосновений в сутки (> 0).
     * @throws InvalidParameterException при некорректных базовых параметрах или неположительном лимите.
     */
    TouchPool(int id, std::string name, double volumeLiters, WaterParameters water,
              Location location, int maxAnimals, int maxTouchesPerDay);

    /// @brief Число прикосновений за текущие сутки.
    int touchesToday() const { return touchesToday_; }

    /// @brief Название типа аквариума.
    std::string type() const override { return "TouchPool"; }
    /// @brief Допускает только безопасных для прикосновений скатов, медуз и черепах.
    /// @param animal Проверяемое животное.
    bool canHost(const Animal& animal) const override;
    /// @brief Регистрирует прикосновение.
    /// @return true, если бассейн открыт и лимит не исчерпан; иначе false.
    bool allowTouch();
    /// @brief Сколько прикосновений ещё разрешено сегодня.
    int remainingTouches() const;
    /// @brief Сбрасывает счётчик прикосновений на новые сутки.
    void resetDay();

private:
    int maxTouchesPerDay_;  ///< Лимит прикосновений в сутки.
    int touchesToday_;      ///< Прикосновений за сегодня.
};

/// @brief Карантинный аквариум для новых и больных животных.
class QuarantineTank : public Tank {
public:
    /**
     * @brief Создаёт карантинный аквариум (закрыт для посетителей).
     * @param id Идентификатор.
     * @param name Название.
     * @param volumeLiters Объём, л.
     * @param water Параметры воды.
     * @param location Расположение.
     * @param maxAnimals Максимум обитателей.
     * @param daysRequired Длительность карантина в днях (> 0).
     * @throws InvalidParameterException при некорректных базовых параметрах или неположительном сроке.
     */
    QuarantineTank(int id, std::string name, double volumeLiters, WaterParameters water,
                   Location location, int maxAnimals, int daysRequired);

    /// @brief Длительность карантина в днях.
    int daysRequired() const { return daysRequired_; }

    /// @brief Название типа аквариума.
    std::string type() const override { return "QuarantineTank"; }
    /**
     * @brief Помещает животное на карантин и запоминает дату поступления.
     * @param animal Поступающее животное.
     * @param date Дата поступления.
     * @throws InvalidParameterException по тем же причинам, что и Tank::addAnimal().
     * @throws TankCapacityExceededException по тем же причинам, что и Tank::addAnimal().
     * @throws InvalidWaterParametersException по тем же причинам, что и Tank::addAnimal().
     * @throws IncompatibleSpeciesException по тем же причинам, что и Tank::addAnimal().
     */
    void admit(std::shared_ptr<Animal> animal, const Date& date);
    /**
     * @brief Можно ли выпустить животное: срок вышел и нет серьёзных заболеваний.
     * @param animalId Идентификатор животного.
     * @param today Текущая дата.
     * @throws AnimalNotFoundException если животное не поступало на карантин.
     */
    bool canRelease(int animalId, const Date& today) const;
    /**
     * @brief Сколько дней карантина осталось.
     * @param animalId Идентификатор животного.
     * @param today Текущая дата.
     * @return Число оставшихся дней (не меньше 0).
     * @throws AnimalNotFoundException если животное не поступало на карантин.
     */
    int daysRemaining(int animalId, const Date& today) const;
    /**
     * @brief Выпустить животное; бросает QuarantineViolationException при досрочном выпуске.
     * @param animalId Идентификатор животного.
     * @param today Текущая дата.
     * @return Выпущенное животное.
     * @throws QuarantineViolationException если срок не вышел или есть серьёзное заболевание.
     * @throws AnimalNotFoundException если животное не поступало на карантин.
     */
    std::shared_ptr<Animal> release(int animalId, const Date& today);

private:
    /// @brief Дата поступления животного на карантин.
    /// @throws AnimalNotFoundException если животное не поступало на карантин.
    const Date& admissionDate(int animalId) const;

    int daysRequired_;                ///< Длительность карантина, дней.
    std::map<int, Date> admissions_;  ///< Даты поступления по идентификатору животного.
};

/// @brief Тематическая экспозиция из нескольких аквариумов.
class Exhibit {
public:
    /**
     * @brief Создаёт экспозицию.
     * @param name Непустое название.
     * @param theme Тема экспозиции.
     * @throws InvalidParameterException если название пустое.
     */
    Exhibit(std::string name, std::string theme);

    /// @brief Название экспозиции.
    const std::string& name() const { return name_; }
    /// @brief Тема экспозиции.
    const std::string& theme() const { return theme_; }
    /// @brief Число зарегистрированных посещений.
    int visitorCount() const { return visitorCount_; }
    /// @brief Аквариумы экспозиции.
    const std::vector<Tank*>& tanks() const { return tanks_; }

    /**
     * @brief Добавляет аквариум (без передачи владения).
     * @param tank Добавляемый аквариум.
     * @throws InvalidParameterException если указатель пуст или аквариум уже в экспозиции.
     */
    void addTank(Tank* tank);
    /// @brief Убирает аквариум из экспозиции.
    /// @param tankId Идентификатор аквариума.
    /// @return true, если аквариум был найден и удалён.
    bool removeTank(int tankId);
    /// @brief Общее число животных во всех аквариумах.
    int totalAnimals() const;
    /// @brief Число различных видов во всех аквариумах.
    int speciesDiversity() const;
    /// @brief true, если открыт хотя бы один аквариум.
    bool isOpen() const;
    /**
     * @brief Регистрирует посещение экспозиции.
     * @throws OceanariumException если все аквариумы закрыты.
     */
    void registerVisit();

private:
    std::string name_;         ///< Название.
    std::string theme_;        ///< Тема.
    std::vector<Tank*> tanks_; ///< Аквариумы (не владеет).
    int visitorCount_;         ///< Число посещений.
};

/// @brief Мониторинг среды: набор датчиков и журнал тревог.
class EnvironmentMonitor {
public:
    /// @brief Создаёт монитор без датчиков и тревог.
    EnvironmentMonitor();

    /// @brief Журнал тревог.
    const std::vector<std::string>& alerts() const { return alerts_; }
    /// @brief Сколько проверок выполнено.
    int checksPerformed() const { return checksPerformed_; }
    /// @brief Датчики монитора (изменяемая ссылка).
    std::vector<Sensor>& sensors() { return sensors_; }

    /// @brief Добавляет датчик.
    /// @param sensor Добавляемый датчик.
    void addSensor(const Sensor& sensor);
    /**
     * @brief Опросить датчики по воде аквариума; вернуть число новых тревог.
     * @details Дополнительно фиксирует тревогу, если аквариуму нужна чистка.
     * @param tank Проверяемый аквариум.
     * @return Число добавленных в журнал тревог.
     */
    int check(const Tank& tank);
    /// @brief Калибрует все датчики.
    void calibrateAll();
    /// @brief Число записей в журнале тревог.
    int alertCount() const;
    /// @brief Очищает журнал тревог.
    void clearAlerts();

private:
    std::vector<Sensor> sensors_;    ///< Датчики.
    std::vector<std::string> alerts_;///< Журнал тревог.
    int checksPerformed_;            ///< Число выполненных проверок.
};

}  // namespace ocean
