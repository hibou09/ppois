/**
 * @file Staff.h
 * @brief Сотрудники океанариума, смены и расписание персонала.
 */
#pragma once

#include <string>
#include <vector>

#include "Animals.h"
#include "Common.h"
#include "Health.h"
#include "Tanks.h"
#include "Visitors.h"

namespace ocean {

/**
 * @brief Базовый сотрудник.
 * @details Хранит идентификатор, имя, базовый оклад и дату найма. От него наследуются
 *          все специализированные должности.
 */
class Employee {
public:
    /**
     * @brief Создаёт сотрудника.
     * @param id Положительный идентификатор.
     * @param name Непустое имя.
     * @param baseSalary Положительный базовый оклад.
     * @param hireDate Дата найма.
     * @throws InvalidParameterException если id не положителен, имя пустое или оклад не положителен.
     */
    Employee(int id, std::string name, double baseSalary, const Date& hireDate);
    /// @brief Виртуальный деструктор для корректного удаления через указатель на базовый класс.
    virtual ~Employee() = default;

    /// @brief Идентификатор сотрудника.
    int id() const { return id_; }
    /// @brief Имя сотрудника.
    const std::string& name() const { return name_; }
    /// @brief Базовый оклад (без надбавок).
    double baseSalary() const { return baseSalary_; }
    /// @brief Дата найма.
    const Date& hireDate() const { return hireDate_; }
    /// @brief Работает ли сотрудник (false после увольнения).
    bool isActive() const { return active_; }

    /// @brief Название должности.
    virtual std::string position() const { return "Employee"; }
    /**
     * @brief Ежемесячная зарплата с учётом надбавок должности.
     * @return Базовый оклад; наследники переопределяют расчёт.
     */
    virtual double monthlySalary() const;

    /**
     * @brief Стаж работы в полных годах.
     * @param today Текущая дата.
     * @return Число полных лет со дня найма, не меньше 0.
     */
    int yearsOfService(const Date& today) const;
    /**
     * @brief Повышает базовый оклад на заданный процент.
     * @param percent Процент повышения, 0 < percent <= 50.
     * @throws InvalidParameterException если процент вне диапазона (0, 50].
     */
    void giveRaise(double percent);
    /// @brief Увольняет сотрудника (active становится false).
    void dismiss();

protected:
    int id_;                ///< Идентификатор.
    std::string name_;      ///< Имя.
    double baseSalary_;     ///< Базовый оклад.
    Date hireDate_;         ///< Дата найма.
    bool active_;           ///< Признак того, что сотрудник работает.
};

/**
 * @brief Аквариумист — обслуживает закреплённые аквариумы.
 * @details Уровень сертификации (1–3) определяет надбавку к зарплате и лимит закреплённых
 *          аквариумов (2 на уровень).
 */
class Aquarist : public Employee {
public:
    /**
     * @brief Создаёт аквариумиста.
     * @param id Идентификатор.
     * @param name Имя.
     * @param baseSalary Базовый оклад.
     * @param hireDate Дата найма.
     * @param certLevel Уровень сертификации, 1..3.
     * @throws InvalidParameterException если параметры сотрудника некорректны или уровень вне [1, 3].
     */
    Aquarist(int id, std::string name, double baseSalary, const Date& hireDate, int certLevel);

    /// @brief Уровень сертификации.
    int certificationLevel() const { return certLevel_; }
    /// @brief Число проведённых осмотров.
    int inspections() const { return inspections_; }
    /// @brief Закреплённые аквариумы.
    const std::vector<Tank*>& assignedTanks() const { return assignedTanks_; }

    /// @brief Название должности ("Aquarist").
    std::string position() const override { return "Aquarist"; }
    /**
     * @brief Зарплата с надбавкой 10% за каждый уровень сертификации выше первого.
     * @return Месячная зарплата.
     */
    double monthlySalary() const override;

    /**
     * @brief Закрепляет аквариум за аквариумистом; лимит — 2 аквариума на уровень сертификации.
     * @param tank Закрепляемый аквариум.
     * @throws InvalidParameterException если указатель нулевой или аквариум уже закреплён.
     * @throws EmployeeNotQualifiedException если превышен лимит аквариумов или для рифового
     *         аквариума не хватает сертификации (нужен уровень 2).
     */
    void assignTank(Tank* tank);
    /**
     * @brief Проверяет, закреплён ли аквариум за сотрудником.
     * @param tankId Идентификатор аквариума.
     * @return true, если аквариум закреплён.
     */
    bool isResponsibleFor(int tankId) const;
    /**
     * @brief Осмотр аквариума.
     * @param tank Осматриваемый аквариум.
     * @return true, если аквариум здоров, не требует чистки и индекс качества воды не ниже 70.
     */
    bool inspect(const Tank& tank);
    /**
     * @brief Чистит аквариум: частичная замена воды (30%) и замена фильтра при необходимости.
     * @param tank Аквариум, закреплённый за сотрудником.
     * @throws EmployeeNotQualifiedException если сотрудник уволен или аквариум за ним не закреплён.
     */
    void cleanTank(Tank& tank);
    /**
     * @brief Повышает уровень сертификации на единицу.
     * @return true, если уровень повышен; false, если уже достигнут максимальный (3).
     */
    bool promote();

private:
    int certLevel_;                    ///< Уровень сертификации (1..3).
    int inspections_;                  ///< Счётчик осмотров.
    std::vector<Tank*> assignedTanks_; ///< Закреплённые аквариумы (без владения).
};

/**
 * @brief Ветеринар.
 * @details Лечить и осматривать животных можно только с действующей лицензией.
 */
class Veterinarian : public Employee {
public:
    /**
     * @brief Создаёт ветеринара.
     * @param id Идентификатор.
     * @param name Имя.
     * @param baseSalary Базовый оклад.
     * @param hireDate Дата найма.
     * @param licenseNumber Непустой номер лицензии.
     * @param licenseExpiry Дата окончания действия лицензии.
     * @param specialization Специализация.
     * @throws InvalidParameterException если параметры сотрудника некорректны или номер лицензии пуст.
     */
    Veterinarian(int id, std::string name, double baseSalary, const Date& hireDate,
                 std::string licenseNumber, const Date& licenseExpiry, std::string specialization);

    /// @brief Номер лицензии.
    const std::string& licenseNumber() const { return licenseNumber_; }
    /// @brief Специализация.
    const std::string& specialization() const { return specialization_; }
    /// @brief Число проведённых осмотров.
    int examinations() const { return examinations_; }

    /// @brief Название должности ("Veterinarian").
    std::string position() const override { return "Veterinarian"; }
    /**
     * @brief Зарплата с надбавкой 30%.
     * @return Месячная зарплата.
     */
    double monthlySalary() const override;

    /**
     * @brief Проверяет действие лицензии на указанную дату.
     * @param today Дата проверки.
     * @return true, если срок лицензии не истёк.
     */
    bool isLicenseValid(const Date& today) const;
    /**
     * @brief Продлевает лицензию.
     * @param newExpiry Новая дата окончания.
     * @throws InvalidDateException если новая дата не позже текущей.
     */
    void renewLicense(const Date& newExpiry);
    /**
     * @brief Осматривает животное и делает запись в его медкарте.
     * @param animal Осматриваемое животное.
     * @param today Дата осмотра.
     * @param diagnosis Диагноз.
     * @param severity Тяжесть состояния.
     * @throws EmployeeNotQualifiedException если ветеринар уволен.
     * @throws LicenseExpiredException если лицензия на дату осмотра недействительна.
     */
    void examine(Animal& animal, const Date& today, const std::string& diagnosis, int severity);
    /**
     * @brief Лечение: выдаёт лекарство и делает запись в медкарте.
     * @param animal Лечимое животное.
     * @param medication Выдаваемое лекарство.
     * @param today Дата лечения.
     * @return Выданная доза, мг.
     * @throws EmployeeNotQualifiedException если ветеринар уволен.
     * @throws LicenseExpiredException если лицензия недействительна.
     * @note Исключения, связанные с выдачей лекарства, передаются из Medication::dispense.
     */
    double treat(Animal& animal, Medication& medication, const Date& today);

private:
    /**
     * @brief Проверяет, что ветеринар работает и имеет действующую лицензию.
     * @param today Дата проверки.
     * @throws EmployeeNotQualifiedException если ветеринар уволен.
     * @throws LicenseExpiredException если лицензия истекла.
     */
    void ensureLicense(const Date& today) const;

    std::string licenseNumber_; ///< Номер лицензии.
    Date licenseExpiry_;        ///< Дата окончания лицензии.
    std::string specialization_;///< Специализация.
    int examinations_;          ///< Счётчик осмотров.
};

/**
 * @brief Водолаз — работает внутри больших аквариумов.
 */
class Diver : public Employee {
public:
    /**
     * @brief Создаёт водолаза.
     * @param id Идентификатор.
     * @param name Имя.
     * @param baseSalary Базовый оклад.
     * @param hireDate Дата найма.
     * @param maxDepthM Максимальная разрешённая глубина, м (положительная).
     * @throws InvalidParameterException если параметры сотрудника некорректны или глубина не положительна.
     */
    Diver(int id, std::string name, double baseSalary, const Date& hireDate, double maxDepthM);

    /// @brief Максимальная глубина погружения, м.
    double maxDepth() const { return maxDepthM_; }
    /// @brief Суммарное время погружений, ч.
    double diveHours() const { return diveHours_; }

    /// @brief Название должности ("Diver").
    std::string position() const override { return "Diver"; }
    /**
     * @brief Зарплата с надбавкой 20%.
     * @return Месячная зарплата.
     */
    double monthlySalary() const override;

    /**
     * @brief Проверяет, позволяет ли сертификация погружение в аквариум.
     * @param tank Океанический аквариум.
     * @return true, если глубина аквариума не превышает максимальную глубину водолаза.
     */
    bool canDiveIn(const OpenOceanTank& tank) const;
    /**
     * @brief Погружение в аквариум.
     * @param tank Аквариум.
     * @param hours Длительность, ч (0 < hours <= 4).
     * @throws InvalidParameterException если длительность вне диапазона (0, 4].
     * @throws EmployeeNotQualifiedException если аквариум глубже сертификации водолаза.
     */
    void dive(OpenOceanTank& tank, double hours);
    /**
     * @brief Повышает допустимую глубину погружения.
     * @param newMaxDepthM Новая максимальная глубина, м.
     * @throws InvalidParameterException если новая глубина не больше текущей.
     */
    void upgradeCertification(double newMaxDepthM);

private:
    double maxDepthM_;  ///< Максимальная глубина, м.
    double diveHours_;  ///< Накопленные часы погружений.
};

/**
 * @brief Дрессировщик морских животных.
 */
class Trainer : public Employee {
public:
    /**
     * @brief Создаёт дрессировщика.
     * @param id Идентификатор.
     * @param name Имя.
     * @param baseSalary Базовый оклад.
     * @param hireDate Дата найма.
     * @param specialty Специализация.
     * @throws InvalidParameterException если параметры сотрудника некорректны.
     */
    Trainer(int id, std::string name, double baseSalary, const Date& hireDate,
            std::string specialty);

    /// @brief Специализация.
    const std::string& specialty() const { return specialty_; }
    /// @brief Число проведённых шоу.
    int showsConducted() const { return showsConducted_; }

    /// @brief Название должности ("Trainer").
    std::string position() const override { return "Trainer"; }

    /**
     * @brief Обучает тюленя трюку.
     * @param seal Тюлень.
     * @param trick Название трюка.
     * @return Результат Seal::learnTrick (true, если трюк выучен).
     */
    bool train(Seal& seal, const std::string& trick);
    /**
     * @brief Проводит шоу: каждый тюлень показывает свои трюки.
     * @param show Шоу.
     * @return Число успешно показанных трюков.
     * @throws InvalidParameterException если шоу отменено или в нём нет исполнителей.
     */
    int conductShow(Show& show);

private:
    std::string specialty_; ///< Специализация.
    int showsConducted_;    ///< Счётчик проведённых шоу.
};

/**
 * @brief Экскурсовод.
 */
class Guide : public Employee {
public:
    /**
     * @brief Создаёт экскурсовода.
     * @param id Идентификатор.
     * @param name Имя.
     * @param baseSalary Базовый оклад.
     * @param hireDate Дата найма.
     * @param languages Языки, на которых говорит экскурсовод (не пустой список).
     * @throws InvalidParameterException если параметры сотрудника некорректны или список языков пуст.
     */
    Guide(int id, std::string name, double baseSalary, const Date& hireDate,
          std::vector<std::string> languages);

    /// @brief Языки экскурсовода.
    const std::vector<std::string>& languages() const { return languages_; }
    /// @brief Число проведённых экскурсий.
    int toursGiven() const { return toursGiven_; }

    /// @brief Название должности ("Guide").
    std::string position() const override { return "Guide"; }
    /**
     * @brief Зарплата с надбавкой 5% за каждый язык сверх первого.
     * @return Месячная зарплата.
     */
    double monthlySalary() const override;

    /**
     * @brief Проверяет, владеет ли экскурсовод языком.
     * @param language Язык.
     * @return true, если язык есть в списке.
     */
    bool speaks(const std::string& language) const;
    /**
     * @brief Добавляет язык в список (повторы игнорируются).
     * @param language Непустое название языка.
     * @throws InvalidParameterException если название пустое.
     */
    void learnLanguage(const std::string& language);
    /**
     * @brief Проводит экскурсию.
     * @param tour Экскурсия, назначенная этому экскурсоводу.
     * @return Размер группы.
     * @throws EmployeeNotQualifiedException если экскурсия назначена другому экскурсоводу.
     * @note Исключения завершения экскурсии передаются из Tour::complete.
     */
    int conductTour(Tour& tour);

private:
    std::vector<std::string> languages_; ///< Языки.
    int toursGiven_;                     ///< Счётчик проведённых экскурсий.
};

/**
 * @brief Кассир.
 */
class Cashier : public Employee {
public:
    /**
     * @brief Создаёт кассира.
     * @param id Идентификатор.
     * @param name Имя.
     * @param baseSalary Базовый оклад.
     * @param hireDate Дата найма.
     * @param registerId Номер кассы.
     * @throws InvalidParameterException если параметры сотрудника некорректны.
     */
    Cashier(int id, std::string name, double baseSalary, const Date& hireDate, int registerId);

    /// @brief Номер кассы.
    int registerId() const { return registerId_; }
    /// @brief Выручка текущей смены.
    double shiftRevenue() const { return shiftRevenue_; }
    /// @brief Число билетов, проданных за смену.
    int ticketsSold() const { return ticketsSold_; }

    /// @brief Название должности ("Cashier").
    std::string position() const override { return "Cashier"; }

    /**
     * @brief Продаёт билет посетителю и учитывает выручку смены.
     * @param office Билетная касса.
     * @param visitor Покупатель.
     * @param type Тип билета.
     * @param date Дата визита.
     * @param membership Абонемент для скидки или nullptr.
     * @param promo Промокод или nullptr.
     * @return Проданный билет.
     * @throws EmployeeNotQualifiedException если кассир уволен.
     * @note Исключения продажи (нехватка средств, неверный промокод и т.п.) передаются из TicketOffice::sell.
     */
    Ticket sellTicket(TicketOffice& office, Visitor& visitor, TicketType type, const Date& date,
                      const Membership* membership = nullptr, PromoCode* promo = nullptr);
    /**
     * @brief Закрывает смену: возвращает выручку и обнуляет счётчики.
     * @return Выручка закрытой смены.
     */
    double closeShift();

private:
    int registerId_;       ///< Номер кассы.
    double shiftRevenue_;  ///< Выручка смены.
    int ticketsSold_;      ///< Число проданных за смену билетов.
};

/**
 * @brief Рабочая смена сотрудника.
 */
class Shift {
public:
    /**
     * @brief Создаёт смену.
     * @param employeeId Идентификатор сотрудника.
     * @param date Дата смены.
     * @param slot Временной интервал смены.
     */
    Shift(int employeeId, const Date& date, const TimeSlot& slot);

    /// @brief Идентификатор сотрудника.
    int employeeId() const { return employeeId_; }
    /// @brief Дата смены.
    const Date& date() const { return date_; }
    /// @brief Временной интервал смены.
    const TimeSlot& slot() const { return slot_; }

    /**
     * @brief Проверяет пересечение с другой сменой того же сотрудника.
     * @param other Другая смена.
     * @return true, если сотрудник, дата совпадают и интервалы пересекаются.
     */
    bool overlaps(const Shift& other) const;
    /**
     * @brief Длительность смены.
     * @return Длительность в часах.
     */
    double hours() const;

private:
    int employeeId_; ///< Идентификатор сотрудника.
    Date date_;      ///< Дата смены.
    TimeSlot slot_;  ///< Временной интервал.
};

/**
 * @brief Расписание смен персонала.
 */
class StaffSchedule {
public:
    /// @brief Создаёт пустое расписание.
    StaffSchedule() = default;

    /// @brief Все смены расписания.
    const std::vector<Shift>& shifts() const { return shifts_; }

    /**
     * @brief Добавляет смену в расписание.
     * @param shift Новая смена.
     * @throws ScheduleConflictException если у сотрудника уже есть пересекающаяся смена.
     */
    void addShift(const Shift& shift);
    /**
     * @brief Суммарные часы сотрудника во всех сменах.
     * @param employeeId Идентификатор сотрудника.
     * @return Сумма часов.
     */
    double totalHours(int employeeId) const;
    /**
     * @brief Смены на указанную дату.
     * @param date Дата.
     * @return Список смен этой даты.
     */
    std::vector<Shift> shiftsOn(const Date& date) const;
    /**
     * @brief Проверяет, работает ли сотрудник в заданную минуту дня.
     * @param employeeId Идентификатор сотрудника.
     * @param date Дата.
     * @param minute Минута от начала суток.
     * @return true, если минута попадает в одну из его смен этого дня.
     */
    bool isWorking(int employeeId, const Date& date, int minute) const;
    /**
     * @brief Удаляет все смены сотрудника.
     * @param employeeId Идентификатор сотрудника.
     * @return Число удалённых смен.
     */
    int removeShiftsOf(int employeeId);

private:
    std::vector<Shift> shifts_; ///< Смены.
};

}  // namespace ocean
