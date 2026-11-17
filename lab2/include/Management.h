/**
 * @file Management.h
 * @brief Обслуживание, финансы и класс-фасад Oceanarium.
 */
#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Feeding.h"
#include "Food.h"
#include "Staff.h"
#include "Tanks.h"
#include "Visitors.h"

namespace ocean {

/// Задача технического обслуживания аквариума.
class MaintenanceTask {
public:
    MaintenanceTask(int id, std::string description, Tank* tank, int priority,
                    const Date& dueDate);

    int id() const { return id_; }
    const std::string& description() const { return description_; }
    Tank* tank() const { return tank_; }
    int priority() const { return priority_; }
    const Date& dueDate() const { return dueDate_; }
    bool isDone() const { return done_; }
    const std::string& completedBy() const { return completedBy_; }

    void complete(const Employee& employee);
    bool isOverdue(const Date& today) const;
    void escalate();

private:
    int id_;
    std::string description_;
    Tank* tank_;
    int priority_;
    Date dueDate_;
    bool done_;
    std::string completedBy_;
};

/// Журнал задач обслуживания.
class MaintenanceLog {
public:
    MaintenanceLog();

    const std::vector<MaintenanceTask>& tasks() const { return tasks_; }

    int schedule(const std::string& description, Tank* tank, int priority, const Date& dueDate);
    MaintenanceTask& find(int taskId);
    void complete(int taskId, const Employee& employee);
    int pendingCount() const;
    int overdueCount(const Date& today) const;
    int highPriorityPending() const;
    int tasksForTank(int tankId) const;

private:
    std::vector<MaintenanceTask> tasks_;
    int nextId_;
};

/// Бюджет океанариума.
class Budget {
public:
    Budget(double initialBalance, double expenseLimit);

    double balance() const { return balance_; }
    double income() const { return income_; }
    double expenses() const { return expenses_; }
    double expenseLimit() const { return expenseLimit_; }

    void addIncome(double amount);
    /// Расход по категории; проверяет лимит и остаток.
    void spend(const std::string& category, double amount);
    double spentOn(const std::string& category) const;
    double profit() const;
    double remainingLimit() const;

private:
    double balance_;
    double income_;
    double expenses_;
    double expenseLimit_;
    std::map<std::string, double> byCategory_;
};

/// Расчёт и выплата зарплаты.
class Payroll {
public:
    explicit Payroll(double bonusPercent = 0.0);

    double totalPaid() const { return totalPaid_; }
    int paymentsCount() const { return paymentsCount_; }
    double bonusPercent() const { return bonusPercent_; }

    void setBonus(double percent);
    double calculate(const Employee& employee) const;
    /// Выплатить зарплату всем активным сотрудникам из бюджета.
    double payAll(const std::vector<Employee*>& employees, Budget& budget);

private:
    double totalPaid_;
    int paymentsCount_;
    double bonusPercent_;
};

/// Океанариум — корневой объект модели.
class Oceanarium {
public:
    Oceanarium(std::string name, std::string city, double initialBalance, double ticketPrice);

    const std::string& name() const { return name_; }
    const std::string& city() const { return city_; }
    TicketOffice& ticketOffice() { return ticketOffice_; }
    FoodStorage& foodStorage() { return foodStorage_; }
    MaintenanceLog& maintenance() { return maintenance_; }
    StaffSchedule& schedule() { return schedule_; }
    FeedingSchedule& feeding() { return feeding_; }
    Budget& budget() { return budget_; }
    Payroll& payroll() { return payroll_; }
    std::vector<Exhibit>& exhibits() { return exhibits_; }

    Tank& addTank(std::unique_ptr<Tank> tank);
    Tank& findTank(int tankId);
    int tankCount() const;
    Employee& hire(std::unique_ptr<Employee> employee);
    Employee& findEmployee(int employeeId);
    void fire(int employeeId);
    Exhibit& addExhibit(const Exhibit& exhibit);

    Tank& locateAnimal(int animalId);
    /// Перевести животное между аквариумами (с откатом при ошибке).
    void transferAnimal(int animalId, int fromTankId, int toTankId);
    int totalAnimals() const;
    int endangeredAnimals() const;
    double dailyFoodRequirement() const;
    /// Провести кормления, назначенные на minute.
    int feedAnimals(int minute, const Date& today);
    /// Суточный цикл: жизнеобеспечение, голод, задачи на уборку. Возвращает число новых задач.
    int runDailyOperations(const Date& today);
    double paySalaries();
    std::string summary() const;

private:
    std::string name_;
    std::string city_;
    std::vector<std::unique_ptr<Tank>> tanks_;
    std::vector<std::unique_ptr<Employee>> employees_;
    std::vector<Exhibit> exhibits_;
    TicketOffice ticketOffice_;
    FoodStorage foodStorage_;
    MaintenanceLog maintenance_;
    StaffSchedule schedule_;
    FeedingSchedule feeding_;
    Budget budget_;
    Payroll payroll_;
};

}  // namespace ocean
