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

/// Базовый сотрудник.
class Employee {
public:
    Employee(int id, std::string name, double baseSalary, const Date& hireDate);
    virtual ~Employee() = default;

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    double baseSalary() const { return baseSalary_; }
    const Date& hireDate() const { return hireDate_; }
    bool isActive() const { return active_; }

    virtual std::string position() const { return "Employee"; }
    virtual double monthlySalary() const;

    int yearsOfService(const Date& today) const;
    void giveRaise(double percent);
    void dismiss();

protected:
    int id_;
    std::string name_;
    double baseSalary_;
    Date hireDate_;
    bool active_;
};

/// Аквариумист — обслуживает закреплённые аквариумы.
class Aquarist : public Employee {
public:
    Aquarist(int id, std::string name, double baseSalary, const Date& hireDate, int certLevel);

    int certificationLevel() const { return certLevel_; }
    int inspections() const { return inspections_; }
    const std::vector<Tank*>& assignedTanks() const { return assignedTanks_; }

    std::string position() const override { return "Aquarist"; }
    double monthlySalary() const override;

    /// Закрепить аквариум; лимит — 2 аквариума на уровень сертификации.
    void assignTank(Tank* tank);
    bool isResponsibleFor(int tankId) const;
    /// Осмотр аквариума: true, если всё в порядке.
    bool inspect(const Tank& tank);
    void cleanTank(Tank& tank);
    bool promote();

private:
    int certLevel_;
    int inspections_;
    std::vector<Tank*> assignedTanks_;
};

/// Ветеринар.
class Veterinarian : public Employee {
public:
    Veterinarian(int id, std::string name, double baseSalary, const Date& hireDate,
                 std::string licenseNumber, const Date& licenseExpiry, std::string specialization);

    const std::string& licenseNumber() const { return licenseNumber_; }
    const std::string& specialization() const { return specialization_; }
    int examinations() const { return examinations_; }

    std::string position() const override { return "Veterinarian"; }
    double monthlySalary() const override;

    bool isLicenseValid(const Date& today) const;
    void renewLicense(const Date& newExpiry);
    void examine(Animal& animal, const Date& today, const std::string& diagnosis, int severity);
    /// Лечение: выдаёт лекарство и делает запись в медкарте; возвращает дозу, мг.
    double treat(Animal& animal, Medication& medication, const Date& today);

private:
    void ensureLicense(const Date& today) const;

    std::string licenseNumber_;
    Date licenseExpiry_;
    std::string specialization_;
    int examinations_;
};

/// Водолаз — работает внутри больших аквариумов.
class Diver : public Employee {
public:
    Diver(int id, std::string name, double baseSalary, const Date& hireDate, double maxDepthM);

    double maxDepth() const { return maxDepthM_; }
    double diveHours() const { return diveHours_; }

    std::string position() const override { return "Diver"; }
    double monthlySalary() const override;

    bool canDiveIn(const OpenOceanTank& tank) const;
    void dive(OpenOceanTank& tank, double hours);
    void upgradeCertification(double newMaxDepthM);

private:
    double maxDepthM_;
    double diveHours_;
};

/// Дрессировщик морских животных.
class Trainer : public Employee {
public:
    Trainer(int id, std::string name, double baseSalary, const Date& hireDate,
            std::string specialty);

    const std::string& specialty() const { return specialty_; }
    int showsConducted() const { return showsConducted_; }

    std::string position() const override { return "Trainer"; }

    bool train(Seal& seal, const std::string& trick);
    /// Провести шоу: каждый тюлень показывает свои трюки; возвращает число трюков.
    int conductShow(Show& show);

private:
    std::string specialty_;
    int showsConducted_;
};

/// Экскурсовод.
class Guide : public Employee {
public:
    Guide(int id, std::string name, double baseSalary, const Date& hireDate,
          std::vector<std::string> languages);

    const std::vector<std::string>& languages() const { return languages_; }
    int toursGiven() const { return toursGiven_; }

    std::string position() const override { return "Guide"; }
    double monthlySalary() const override;

    bool speaks(const std::string& language) const;
    void learnLanguage(const std::string& language);
    /// Провести экскурсию; возвращает размер группы.
    int conductTour(Tour& tour);

private:
    std::vector<std::string> languages_;
    int toursGiven_;
};

/// Кассир.
class Cashier : public Employee {
public:
    Cashier(int id, std::string name, double baseSalary, const Date& hireDate, int registerId);

    int registerId() const { return registerId_; }
    double shiftRevenue() const { return shiftRevenue_; }
    int ticketsSold() const { return ticketsSold_; }

    std::string position() const override { return "Cashier"; }

    Ticket sellTicket(TicketOffice& office, Visitor& visitor, TicketType type, const Date& date,
                      const Membership* membership = nullptr, PromoCode* promo = nullptr);
    /// Закрыть смену: вернуть выручку и обнулить счётчики.
    double closeShift();

private:
    int registerId_;
    double shiftRevenue_;
    int ticketsSold_;
};

/// Рабочая смена сотрудника.
class Shift {
public:
    Shift(int employeeId, const Date& date, const TimeSlot& slot);

    int employeeId() const { return employeeId_; }
    const Date& date() const { return date_; }
    const TimeSlot& slot() const { return slot_; }

    bool overlaps(const Shift& other) const;
    double hours() const;

private:
    int employeeId_;
    Date date_;
    TimeSlot slot_;
};

/// Расписание смен.
class StaffSchedule {
public:
    StaffSchedule() = default;

    const std::vector<Shift>& shifts() const { return shifts_; }

    void addShift(const Shift& shift);
    double totalHours(int employeeId) const;
    std::vector<Shift> shiftsOn(const Date& date) const;
    bool isWorking(int employeeId, const Date& date, int minute) const;
    int removeShiftsOf(int employeeId);

private:
    std::vector<Shift> shifts_;
};

}  // namespace ocean
