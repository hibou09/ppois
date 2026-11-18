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

/**
 * @brief Тип питания вида.
 */
enum class Diet {
    Herbivore,   ///< Травоядное.
    Carnivore,   ///< Хищник, питается животной пищей.
    Omnivore,    ///< Всеядное.
    Planktivore  ///< Питается планктоном.
};

/**
 * @brief Природоохранный статус вида.
 */
enum class ConservationStatus {
    LeastConcern,          ///< Вызывает наименьшие опасения.
    NearThreatened,        ///< Близок к угрожаемому положению.
    Vulnerable,            ///< Уязвимый.
    Endangered,            ///< Находится под угрозой исчезновения.
    CriticallyEndangered   ///< На грани исчезновения.
};

/**
 * @brief Биологический вид.
 */
class Species {
public:
    /**
     * @brief Создаёт описание вида.
     * @param name Обычное название.
     * @param latinName Латинское название.
     * @param diet Тип питания.
     * @param requirements Требования вида к воде.
     * @param predatory Признак хищника.
     * @param status Природоохранный статус.
     * @throws InvalidParameterException если название пусто.
     */
    Species(std::string name, std::string latinName, Diet diet, WaterRequirements requirements,
            bool predatory, ConservationStatus status);

    /// @brief Обычное название вида.
    const std::string& name() const { return name_; }
    /// @brief Латинское название вида.
    const std::string& latinName() const { return latinName_; }
    /// @brief Тип питания.
    Diet diet() const { return diet_; }
    /// @brief Требования вида к параметрам воды.
    const WaterRequirements& requirements() const { return requirements_; }
    /// @brief Является ли вид хищным.
    bool isPredatory() const { return predatory_; }
    /// @brief Природоохранный статус.
    ConservationStatus status() const { return status_; }

    /**
     * @brief Находится ли вид под угрозой исчезновения.
     * @return true для статусов Endangered и CriticallyEndangered.
     */
    bool isEndangered() const;
    /**
     * @brief Можно ли держать два вида в одном аквариуме.
     * @details Требования к воде должны пересекаться; разные виды уживаются,
     *          только если оба хищные или оба нехищные.
     * @param other Другой вид.
     * @return true, если виды совместимы.
     */
    bool canCoexistWith(const Species& other) const;

private:
    std::string name_;               ///< Обычное название.
    std::string latinName_;          ///< Латинское название.
    Diet diet_;                      ///< Тип питания.
    WaterRequirements requirements_; ///< Требования к воде.
    bool predatory_;                 ///< Признак хищника.
    ConservationStatus status_;      ///< Природоохранный статус.
};

/**
 * @brief Абстрактный обитатель океанариума.
 */
class Animal {
public:
    /**
     * @brief Создаёт животное.
     * @param id Уникальный номер (> 0).
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах (>= 0).
     * @param weightKg Масса, кг (> 0).
     * @throws InvalidParameterException если номер не положителен, кличка пуста,
     *         возраст отрицателен или масса не положительна.
     */
    Animal(int id, std::string name, Species species, int ageYears, double weightKg);
    /// @brief Виртуальный деструктор для корректного удаления через базовый указатель.
    virtual ~Animal() = default;

    /// @brief Уникальный номер животного.
    int id() const { return id_; }
    /// @brief Кличка.
    const std::string& name() const { return name_; }
    /// @brief Вид животного.
    const Species& species() const { return species_; }
    /// @brief Возраст в годах.
    int ageYears() const { return ageYears_; }
    /// @brief Масса, кг.
    double weightKg() const { return weightKg_; }
    /// @brief Уровень голода (0..100).
    double hunger() const { return hunger_; }
    /// @brief Медицинская карта (для изменения).
    HealthRecord& health() { return health_; }
    /// @brief Медицинская карта (только чтение).
    const HealthRecord& health() const { return health_; }

    /**
     * @brief Суточная норма корма.
     * @return Масса корма, кг.
     */
    virtual double dailyFoodKg() const = 0;
    /**
     * @brief Название категории животного.
     * @return Строка вроде «Fish» или «Seal».
     */
    virtual std::string kind() const = 0;
    /**
     * @brief Краткое описание животного.
     * @return Строка «категория кличка (вид, возраст)».
     */
    virtual std::string describe() const;
    /**
     * @brief Прошли сутки: растёт голод.
     */
    virtual void passDay();

    /**
     * @brief Кормит животное.
     * @details Голод снижается пропорционально отношению kg к суточной норме;
     *          при кормлении сытого животного масса немного растёт (перекорм).
     * @param food Партия корма.
     * @param kg Количество, кг (> 0).
     * @param today Текущая дата.
     * @throws InvalidParameterException если kg <= 0.
     * @throws ExpiredProductException если корм просрочен.
     * @throws InsufficientFoodException если корма в партии не хватает.
     */
    void feed(FoodItem& food, double kg, const Date& today);
    /**
     * @brief Проголодалось ли животное.
     * @return true, если голод не меньше 50.
     */
    bool isHungry() const;
    /**
     * @brief Увеличивает возраст на один год.
     */
    void celebrateBirthday();
    /**
     * @brief Подходит ли вода для вида.
     * @param water Параметры воды.
     * @return true, если вода удовлетворяет требованиям вида.
     */
    bool canLiveIn(const WaterParameters& water) const;
    /**
     * @brief Можно ли держать с другим животным в одном аквариуме.
     * @param other Другое животное.
     * @return true, если виды совместимы.
     */
    bool canShareTankWith(const Animal& other) const;

protected:
    /**
     * @brief Увеличивает голод с ограничением диапазоном 0..100.
     * @param amount Приращение (может быть отрицательным).
     */
    void increaseHunger(double amount);

    int id_;              ///< Уникальный номер.
    std::string name_;    ///< Кличка.
    Species species_;     ///< Вид.
    int ageYears_;        ///< Возраст в годах.
    double weightKg_;     ///< Масса, кг.
    double hunger_;       ///< Уровень голода (0..100).
    HealthRecord health_; ///< Медицинская карта.
};

/**
 * @brief Рыба.
 */
class Fish : public Animal {
public:
    /**
     * @brief Создаёт рыбу.
     * @param id Уникальный номер.
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах.
     * @param weightKg Масса, кг.
     * @param lengthCm Длина, см (> 0).
     * @param schooling Живёт ли стаей.
     * @throws InvalidParameterException если длина не положительна или не прошла проверка Animal.
     */
    Fish(int id, std::string name, Species species, int ageYears, double weightKg,
         double lengthCm, bool schooling);

    /// @brief Длина, см.
    double lengthCm() const { return lengthCm_; }
    /// @brief Стайная ли рыба.
    bool isSchooling() const { return schooling_; }

    /**
     * @brief Суточная норма корма (3% массы).
     * @return Масса корма, кг.
     */
    double dailyFoodKg() const override;
    /// @brief Категория животного: «Fish».
    std::string kind() const override { return "Fish"; }
    /**
     * @brief Крейсерская скорость.
     * @return Скорость, м/с.
     */
    double swimSpeed() const;
    /**
     * @brief Увеличивает длину рыбы.
     * @param cm Прирост, см (> 0).
     * @throws InvalidParameterException если cm <= 0.
     */
    void grow(double cm);

protected:
    double lengthCm_;  ///< Длина, см.
    bool schooling_;   ///< Признак стайной рыбы.
};

/**
 * @brief Акула.
 */
class Shark : public Fish {
public:
    /**
     * @brief Создаёт акулу.
     * @param id Уникальный номер.
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах.
     * @param weightKg Масса, кг.
     * @param lengthCm Длина, см.
     * @param biteForceN Сила укуса, Н (> 0).
     * @throws InvalidParameterException если сила укуса не положительна или не прошла проверка Fish.
     */
    Shark(int id, std::string name, Species species, int ageYears, double weightKg,
          double lengthCm, double biteForceN);

    /// @brief Сила укуса, Н.
    double biteForce() const { return biteForceN_; }
    /// @brief Число потерянных зубов.
    int teethLost() const { return teethLost_; }

    /**
     * @brief Суточная норма корма (2% массы).
     * @return Масса корма, кг.
     */
    double dailyFoodKg() const override;
    /// @brief Категория животного: «Shark».
    std::string kind() const override { return "Shark"; }
    /**
     * @brief Опасна ли акула для другого животного.
     * @param other Другое животное.
     * @return true, если оно другое, не акула и легче половины массы акулы.
     */
    bool isDangerousTo(const Animal& other) const;
    /**
     * @brief Фиксирует потерю зубов.
     * @param count Число зубов (> 0).
     * @throws InvalidParameterException если count <= 0.
     */
    void loseTeeth(int count);

private:
    double biteForceN_;  ///< Сила укуса, Н.
    int teethLost_;      ///< Число потерянных зубов.
};

/**
 * @brief Скат.
 */
class Ray : public Fish {
public:
    /**
     * @brief Создаёт ската.
     * @param id Уникальный номер.
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах.
     * @param weightKg Масса, кг.
     * @param wingspanCm Размах «крыльев», см (> 0).
     * @param venomous Есть ли ядовитый шип.
     * @throws InvalidParameterException если размах не положителен или не прошла проверка Fish.
     */
    Ray(int id, std::string name, Species species, int ageYears, double weightKg,
        double wingspanCm, bool venomous);

    /// @brief Размах «крыльев», см.
    double wingspan() const { return wingspanCm_; }
    /// @brief Есть ли ядовитый шип.
    bool isVenomous() const { return venomous_; }

    /// @brief Категория животного: «Ray».
    std::string kind() const override { return "Ray"; }
    /**
     * @brief Безопасен ли скат для контакта с посетителями.
     * @return true, если скат неядовит или шип обрезан.
     */
    bool isSafeForTouch() const;
    /**
     * @brief Обрезает ядовитый шип.
     * @throws InvalidParameterException если у ската нет ядовитого шипа.
     */
    void clipBarb();

private:
    double wingspanCm_;  ///< Размах «крыльев», см.
    bool venomous_;      ///< Есть ли ядовитый шип.
    bool barbClipped_;   ///< Обрезан ли шип.
};

/**
 * @brief Морская черепаха.
 */
class Turtle : public Animal {
public:
    /**
     * @brief Создаёт черепаху.
     * @param id Уникальный номер.
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах.
     * @param weightKg Масса, кг.
     * @param shellLengthCm Длина панциря, см (> 0).
     * @throws InvalidParameterException если длина панциря не положительна или не прошла проверка Animal.
     */
    Turtle(int id, std::string name, Species species, int ageYears, double weightKg,
           double shellLengthCm);

    /// @brief Длина панциря, см.
    double shellLength() const { return shellLengthCm_; }
    /// @brief Сколько часов черепаха грелась сегодня.
    double baskingHoursToday() const { return baskingHoursToday_; }

    /**
     * @brief Суточная норма корма (1,5% массы).
     * @return Масса корма, кг.
     */
    double dailyFoodKg() const override;
    /// @brief Категория животного: «Turtle».
    std::string kind() const override { return "Turtle"; }
    /**
     * @brief Прошли сутки: растёт голод и обнуляется счётчик обогрева.
     */
    void passDay() override;
    /**
     * @brief Записывает время обогрева под лампой.
     * @param hours Часы (> 0, суммарно за день не более 12).
     * @throws InvalidParameterException если часы некорректны или превышен дневной лимит.
     */
    void bask(double hours);
    /**
     * @brief Нужен ли черепахе ещё обогрев.
     * @return true, если сегодня она грелась меньше 2 часов.
     */
    bool needsBasking() const;
    /**
     * @brief Оставшаяся продолжительность жизни.
     * @return Число лет до kLifespanYears (не меньше 0).
     */
    int remainingLifespan() const;

    /// Ожидаемая продолжительность жизни, лет.
    static const int kLifespanYears = 80;

private:
    double shellLengthCm_;      ///< Длина панциря, см.
    double baskingHoursToday_;  ///< Часы обогрева за сегодня.
};

/**
 * @brief Осьминог.
 */
class Octopus : public Animal {
public:
    /**
     * @brief Создаёт осьминога с восемью щупальцами.
     * @param id Уникальный номер.
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах.
     * @param weightKg Масса, кг.
     * @param intelligence Уровень интеллекта (0..100).
     * @throws InvalidParameterException если интеллект вне диапазона или не прошла проверка Animal.
     */
    Octopus(int id, std::string name, Species species, int ageYears, double weightKg,
            int intelligence);

    /// @brief Число щупалец.
    int armCount() const { return armCount_; }
    /// @brief Уровень интеллекта (0..100).
    int intelligence() const { return intelligence_; }
    /// @brief Число попыток побега.
    int escapeAttempts() const { return escapeAttempts_; }

    /**
     * @brief Суточная норма корма (4% массы).
     * @return Масса корма, кг.
     */
    double dailyFoodKg() const override;
    /// @brief Категория животного: «Octopus».
    std::string kind() const override { return "Octopus"; }
    /**
     * @brief Решает головоломку (элемент обогащения среды).
     * @details Успех, если интеллект не меньше difficulty * 10; при успехе интеллект растёт на 2.
     * @param difficulty Сложность 1..10.
     * @return true, если головоломка решена.
     * @throws InvalidParameterException если сложность вне диапазона 1..10.
     */
    bool solvePuzzle(int difficulty);
    /**
     * @brief Попытка побега; успешна, если крышка не закреплена.
     * @param lidSecured Закреплена ли крышка аквариума.
     * @return true, если побег удался.
     */
    bool attemptEscape(bool lidSecured);
    /**
     * @brief Теряет одно щупальце.
     * @throws InvalidParameterException если щупалец не осталось.
     */
    void loseArm();
    /**
     * @brief Отращивает одно щупальце (не больше восьми).
     */
    void regenerateArm();

private:
    int armCount_;        ///< Число щупалец.
    int intelligence_;    ///< Уровень интеллекта.
    int escapeAttempts_;  ///< Число попыток побега.
};

/**
 * @brief Медуза.
 */
class Jellyfish : public Animal {
public:
    /**
     * @brief Создаёт медузу.
     * @param id Уникальный номер.
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах.
     * @param weightKg Масса, кг.
     * @param bellDiameterCm Диаметр купола, см (> 0).
     * @param stinging Жалит ли медуза.
     * @throws InvalidParameterException если диаметр не положителен или не прошла проверка Animal.
     */
    Jellyfish(int id, std::string name, Species species, int ageYears, double weightKg,
              double bellDiameterCm, bool stinging);

    /// @brief Диаметр купола, см.
    double bellDiameter() const { return bellDiameterCm_; }
    /// @brief Жалит ли медуза.
    bool isStinging() const { return stinging_; }

    /**
     * @brief Суточная норма корма (зависит от диаметра купола, не меньше 1 г).
     * @return Масса корма, кг.
     */
    double dailyFoodKg() const override;
    /// @brief Категория животного: «Jellyfish».
    std::string kind() const override { return "Jellyfish"; }
    /**
     * @brief Частота пульсаций купола при заданной температуре.
     * @param waterTemperature Температура воды, °C.
     * @return Пульсаций в минуту (не меньше 10).
     */
    double pulseRate(double waterTemperature) const;
    /**
     * @brief Безопасна ли медуза для контакта.
     * @return true, если медуза не жалит.
     */
    bool isSafeForTouch() const;

private:
    double bellDiameterCm_;  ///< Диаметр купола, см.
    bool stinging_;          ///< Признак жалящей медузы.
};

/**
 * @brief Пингвин.
 */
class Penguin : public Animal {
public:
    /**
     * @brief Создаёт пингвина.
     * @param id Уникальный номер.
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах.
     * @param weightKg Масса, кг.
     * @param colony Название колонии.
     * @throws InvalidParameterException если не прошла проверка Animal.
     */
    Penguin(int id, std::string name, Species species, int ageYears, double weightKg,
            std::string colony);

    /// @brief Название колонии.
    const std::string& colony() const { return colony_; }
    /// @brief Линяет ли пингвин сейчас.
    bool isMolting() const { return molting_; }
    /// @brief Сколько часов пингвин плавал сегодня.
    double swimHoursToday() const { return swimHoursToday_; }

    /**
     * @brief Суточная норма корма (10% массы, при линьке 15%).
     * @return Масса корма, кг.
     */
    double dailyFoodKg() const override;
    /// @brief Категория животного: «Penguin».
    std::string kind() const override { return "Penguin"; }
    /**
     * @brief Прошли сутки: растёт голод и обнуляется счётчик плавания.
     */
    void passDay() override;
    /**
     * @brief Пингвин плавает; растёт голод.
     * @param hours Часы плавания (> 0).
     * @throws InvalidParameterException если пингвин линяет или hours <= 0.
     */
    void swim(double hours);
    /// @brief Начинает линьку.
    void startMolt();
    /// @brief Заканчивает линьку.
    void finishMolt();
    /**
     * @brief Достаточно ли пингвин двигался.
     * @return true, если он линяет или плавал не меньше часа.
     */
    bool hadEnoughExercise() const;

private:
    std::string colony_;     ///< Название колонии.
    double swimHoursToday_;  ///< Часы плавания за сегодня.
    bool molting_;           ///< Идёт ли линька.
};

/**
 * @brief Тюлень — участник шоу.
 */
class Seal : public Animal {
public:
    /**
     * @brief Создаёт тюленя.
     * @param id Уникальный номер.
     * @param name Кличка.
     * @param species Вид.
     * @param ageYears Возраст в годах.
     * @param weightKg Масса, кг.
     * @param blubberCm Толщина жирового слоя, см.
     * @throws InvalidParameterException если не прошла проверка Animal.
     */
    Seal(int id, std::string name, Species species, int ageYears, double weightKg,
         double blubberCm);

    /// @brief Выученные трюки.
    const std::vector<std::string>& tricks() const { return tricks_; }
    /// @brief Число выступлений.
    int performances() const { return performances_; }

    /**
     * @brief Суточная норма корма (6% массы).
     * @return Масса корма, кг.
     */
    double dailyFoodKg() const override;
    /// @brief Категория животного: «Seal».
    std::string kind() const override { return "Seal"; }
    /**
     * @brief Обучает трюку.
     * @param trick Название трюка.
     * @return true, если трюк новый; false, если тюлень его уже знает.
     * @throws InvalidParameterException если название пусто.
     */
    bool learnTrick(const std::string& trick);
    /**
     * @brief Проверяет, знает ли тюлень трюк.
     * @param trick Название трюка.
     * @return true, если трюк выучен.
     */
    bool knowsTrick(const std::string& trick) const;
    /**
     * @brief Выполняет трюк; растёт число выступлений и голод.
     * @param trick Название трюка.
     * @return true, если трюк выполнен; false, если тюлень его не знает.
     */
    bool perform(const std::string& trick);

private:
    std::vector<std::string> tricks_;  ///< Выученные трюки.
    double blubberCm_;                 ///< Толщина жирового слоя, см.
    int performances_;                 ///< Число выступлений.
};

/**
 * @brief Колония кораллов.
 */
class Coral {
public:
    /**
     * @brief Создаёт колонию кораллов.
     * @param speciesName Название вида.
     * @param sizeCm2 Площадь, см² (> 0).
     * @param healthPercent Здоровье в процентах (0..100); ниже 50 колония считается обесцвеченной.
     * @throws InvalidParameterException если название пусто, площадь не положительна
     *         или здоровье вне диапазона 0..100.
     */
    Coral(std::string speciesName, double sizeCm2, double healthPercent = 100.0);

    /// @brief Название вида кораллов.
    const std::string& speciesName() const { return speciesName_; }
    /// @brief Площадь колонии, см².
    double size() const { return sizeCm2_; }
    /// @brief Здоровье колонии, %.
    double healthPercent() const { return healthPercent_; }
    /// @brief Обесцвечена ли колония.
    bool isBleached() const { return bleached_; }

    /**
     * @brief Рост за сутки при данных параметрах воды.
     * @details Выше kBleachingTemperature здоровье падает на 10%, при падении ниже 50% колония
     *          обесцвечивается; иначе необесцвеченная колония растёт на 2%.
     * @param water Параметры воды.
     * @return Новая площадь, см².
     */
    double grow(const WaterParameters& water);
    /**
     * @brief Отделяет фрагмент для размножения.
     * @param cm2 Площадь фрагмента, см² (> 0 и меньше площади колонии).
     * @return Новая колония-фрагмент с тем же здоровьем.
     * @throws InvalidParameterException если площадь фрагмента некорректна.
     */
    Coral fragment(double cm2);
    /**
     * @brief Восстанавливает здоровье на 20%; при достижении 50% обесцвечивание снимается.
     */
    void recover();

    /// Температура воды, выше которой кораллы обесцвечиваются, °C.
    static constexpr double kBleachingTemperature = 29.0;

private:
    std::string speciesName_;  ///< Название вида.
    double sizeCm2_;           ///< Площадь колонии, см².
    double healthPercent_;     ///< Здоровье, %.
    bool bleached_;            ///< Признак обесцвечивания.
};

}  // namespace ocean
