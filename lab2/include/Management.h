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

/**
 * @brief Задача технического обслуживания аквариума.
 */
class MaintenanceTask {
public:
    /**
     * @brief Создаёт задачу.
     * @param id Идентификатор задачи.
     * @param description Непустое описание.
     * @param tank Аквариум, к которому относится задача (может быть nullptr).
     * @param priority Приоритет от 1 до 5.
     * @param dueDate Срок выполнения.
     * @throws InvalidParameterException если описание пустое или приоритет вне [1, 5].
     */
    MaintenanceTask(int id, std::string description, Tank* tank, int priority,
                    const Date& dueDate);

    /// @brief Идентификатор задачи.
    int id() const { return id_; }
    /// @brief Описание задачи.
    const std::string& description() const { return description_; }
    /// @brief Аквариум задачи (без владения, может быть nullptr).
    Tank* tank() const { return tank_; }
    /// @brief Приоритет (1..5).
    int priority() const { return priority_; }
    /// @brief Срок выполнения.
    const Date& dueDate() const { return dueDate_; }
    /// @brief Выполнена ли задача.
    bool isDone() const { return done_; }
    /// @brief Имя сотрудника, выполнившего задачу (пусто, пока не выполнена).
    const std::string& completedBy() const { return completedBy_; }

    /**
     * @brief Отмечает задачу выполненной сотрудником.
     * @param employee Исполнитель.
     * @throws EmployeeNotQualifiedException если сотрудник уволен.
     * @throws InvalidParameterException если задача уже выполнена.
     */
    void complete(const Employee& employee);
    /**
     * @brief Проверяет просроченность.
     * @param today Текущая дата.
     * @return true, если задача не выполнена и срок прошёл.
     */
    bool isOverdue(const Date& today) const;
    /// @brief Повышает приоритет на единицу, но не выше 5.
    void escalate();

private:
    int id_;                   ///< Идентификатор.
    std::string description_;  ///< Описание.
    Tank* tank_;               ///< Аквариум (без владения).
    int priority_;             ///< Приоритет 1..5.
    Date dueDate_;             ///< Срок выполнения.
    bool done_;                ///< Признак выполнения.
    std::string completedBy_;  ///< Имя исполнителя.
};

/**
 * @brief Журнал задач обслуживания.
 */
class MaintenanceLog {
public:
    /// @brief Создаёт пустой журнал (нумерация задач начинается с 1).
    MaintenanceLog();

    /// @brief Все задачи журнала.
    const std::vector<MaintenanceTask>& tasks() const { return tasks_; }

    /**
     * @brief Создаёт и записывает новую задачу.
     * @param description Описание.
     * @param tank Аквариум или nullptr.
     * @param priority Приоритет 1..5.
     * @param dueDate Срок выполнения.
     * @return Идентификатор созданной задачи.
     * @throws InvalidParameterException если описание пустое или приоритет вне [1, 5].
     */
    int schedule(const std::string& description, Tank* tank, int priority, const Date& dueDate);
    /**
     * @brief Находит задачу по идентификатору.
     * @param taskId Идентификатор задачи.
     * @return Ссылка на задачу.
     * @throws TaskNotFoundException если задачи с таким идентификатором нет.
     */
    MaintenanceTask& find(int taskId);
    /**
     * @brief Отмечает задачу выполненной.
     * @param taskId Идентификатор задачи.
     * @param employee Исполнитель.
     * @throws TaskNotFoundException если задача не найдена.
     * @throws EmployeeNotQualifiedException если сотрудник уволен.
     * @throws InvalidParameterException если задача уже выполнена.
     */
    void complete(int taskId, const Employee& employee);
    /// @brief Число невыполненных задач.
    int pendingCount() const;
    /**
     * @brief Число просроченных задач.
     * @param today Текущая дата.
     * @return Количество невыполненных задач со сроком в прошлом.
     */
    int overdueCount(const Date& today) const;
    /// @brief Число невыполненных задач с приоритетом 4 и выше.
    int highPriorityPending() const;
    /**
     * @brief Число задач (любых) по аквариуму.
     * @param tankId Идентификатор аквариума.
     * @return Количество задач, связанных с этим аквариумом.
     */
    int tasksForTank(int tankId) const;

private:
    std::vector<MaintenanceTask> tasks_; ///< Задачи.
    int nextId_;                         ///< Следующий свободный идентификатор.
};

/**
 * @brief Бюджет океанариума.
 */
class Budget {
public:
    /**
     * @brief Создаёт бюджет.
     * @param initialBalance Начальный остаток (не отрицательный).
     * @param expenseLimit Общий лимит расходов (положительный).
     * @throws InvalidParameterException если остаток отрицателен или лимит не положителен.
     */
    Budget(double initialBalance, double expenseLimit);

    /// @brief Текущий остаток.
    double balance() const { return balance_; }
    /// @brief Суммарные доходы.
    double income() const { return income_; }
    /// @brief Суммарные расходы.
    double expenses() const { return expenses_; }
    /// @brief Лимит расходов.
    double expenseLimit() const { return expenseLimit_; }

    /**
     * @brief Добавляет доход.
     * @param amount Положительная сумма.
     * @throws InvalidParameterException если сумма не положительна.
     */
    void addIncome(double amount);
    /**
     * @brief Расход по категории; проверяет лимит и остаток.
     * @param category Непустая категория расхода.
     * @param amount Положительная сумма.
     * @throws InvalidParameterException если категория пуста или сумма не положительна.
     * @throws BudgetExceededException если расходы превысят лимит.
     * @throws InsufficientFundsException если на счёте недостаточно средств.
     */
    void spend(const std::string& category, double amount);
    /**
     * @brief Сумма расходов по категории.
     * @param category Категория.
     * @return Потраченная сумма (0, если расходов не было).
     */
    double spentOn(const std::string& category) const;
    /// @brief Прибыль: доходы минус расходы.
    double profit() const;
    /// @brief Неиспользованный остаток лимита расходов.
    double remainingLimit() const;

private:
    double balance_;      ///< Остаток средств.
    double income_;       ///< Суммарный доход.
    double expenses_;     ///< Суммарные расходы.
    double expenseLimit_; ///< Лимит расходов.
    std::map<std::string, double> byCategory_; ///< Расходы по категориям.
};

/**
 * @brief Расчёт и выплата зарплаты.
 */
class Payroll {
public:
    /**
     * @brief Создаёт расчёт зарплаты.
     * @param bonusPercent Общая премия в процентах, 0..100.
     * @throws InvalidParameterException если премия вне [0, 100].
     */
    explicit Payroll(double bonusPercent = 0.0);

    /// @brief Всего выплачено.
    double totalPaid() const { return totalPaid_; }
    /// @brief Число выплат сотрудникам.
    int paymentsCount() const { return paymentsCount_; }
    /// @brief Текущая премия, %.
    double bonusPercent() const { return bonusPercent_; }

    /**
     * @brief Устанавливает премию.
     * @param percent Премия в процентах, 0..100.
     * @throws InvalidParameterException если значение вне [0, 100].
     */
    void setBonus(double percent);
    /**
     * @brief Рассчитывает выплату сотруднику с учётом премии.
     * @param employee Сотрудник.
     * @return Месячная зарплата сотрудника, увеличенная на премию.
     */
    double calculate(const Employee& employee) const;
    /**
     * @brief Выплачивает зарплату всем активным сотрудникам из бюджета (всё или ничего).
     * @param employees Список сотрудников (нулевые указатели и уволенные пропускаются).
     * @param budget Бюджет, из которого списывается сумма в категории "salary".
     * @return Выплаченная сумма; 0, если платить некому.
     * @throws BudgetExceededException если превышен лимит расходов.
     * @throws InsufficientFundsException если денег в бюджете не хватает.
     */
    double payAll(const std::vector<Employee*>& employees, Budget& budget);

private:
    double totalPaid_;     ///< Всего выплачено.
    int paymentsCount_;    ///< Число выплат.
    double bonusPercent_;  ///< Премия, %.
};

/**
 * @brief Океанариум — корневой объект модели.
 * @details Владеет аквариумами и сотрудниками и объединяет кассу, склад корма, журнал
 *          обслуживания, расписания, бюджет и расчёт зарплаты.
 */
class Oceanarium {
public:
    /**
     * @brief Создаёт океанариум.
     * @param name Непустое название.
     * @param city Город.
     * @param initialBalance Начальный баланс бюджета (лимит расходов — 10 балансов).
     * @param ticketPrice Базовая цена билета.
     * @throws InvalidParameterException если название пустое или параметры кассы/бюджета некорректны.
     */
    Oceanarium(std::string name, std::string city, double initialBalance, double ticketPrice);

    /// @brief Название.
    const std::string& name() const { return name_; }
    /// @brief Город.
    const std::string& city() const { return city_; }
    /// @brief Билетная касса.
    TicketOffice& ticketOffice() { return ticketOffice_; }
    /// @brief Склад корма.
    FoodStorage& foodStorage() { return foodStorage_; }
    /// @brief Журнал обслуживания.
    MaintenanceLog& maintenance() { return maintenance_; }
    /// @brief Расписание смен персонала.
    StaffSchedule& schedule() { return schedule_; }
    /// @brief Расписание кормлений.
    FeedingSchedule& feeding() { return feeding_; }
    /// @brief Бюджет.
    Budget& budget() { return budget_; }
    /// @brief Расчёт зарплаты.
    Payroll& payroll() { return payroll_; }
    /// @brief Экспозиции.
    std::vector<Exhibit>& exhibits() { return exhibits_; }

    /**
     * @brief Добавляет аквариум (океанариум становится его владельцем).
     * @param tank Аквариум.
     * @return Ссылка на добавленный аквариум.
     * @throws InvalidParameterException если указатель нулевой или аквариум с таким id уже есть.
     */
    Tank& addTank(std::unique_ptr<Tank> tank);
    /**
     * @brief Находит аквариум по идентификатору.
     * @param tankId Идентификатор.
     * @return Ссылка на аквариум.
     * @throws TankNotFoundException если аквариума нет.
     */
    Tank& findTank(int tankId);
    /// @brief Число аквариумов.
    int tankCount() const;
    /**
     * @brief Нанимает сотрудника (океанариум становится его владельцем).
     * @param employee Сотрудник.
     * @return Ссылка на добавленного сотрудника.
     * @throws InvalidParameterException если указатель нулевой или сотрудник с таким id уже есть.
     */
    Employee& hire(std::unique_ptr<Employee> employee);
    /**
     * @brief Находит сотрудника по идентификатору.
     * @param employeeId Идентификатор.
     * @return Ссылка на сотрудника.
     * @throws EmployeeNotFoundException если сотрудника нет.
     */
    Employee& findEmployee(int employeeId);
    /**
     * @brief Увольняет сотрудника и удаляет его смены из расписания.
     * @param employeeId Идентификатор.
     * @throws EmployeeNotFoundException если сотрудника нет.
     */
    void fire(int employeeId);
    /**
     * @brief Добавляет экспозицию.
     * @param exhibit Экспозиция (копируется).
     * @return Ссылка на добавленную экспозицию.
     */
    Exhibit& addExhibit(const Exhibit& exhibit);

    /**
     * @brief Находит аквариум, в котором живёт животное.
     * @param animalId Идентификатор животного.
     * @return Ссылка на аквариум.
     * @throws AnimalNotFoundException если животное не найдено ни в одном аквариуме.
     */
    Tank& locateAnimal(int animalId);
    /**
     * @brief Переводит животное между аквариумами (с откатом при ошибке).
     * @param animalId Идентификатор животного.
     * @param fromTankId Аквариум-источник.
     * @param toTankId Аквариум-приёмник.
     * @throws InvalidParameterException если аквариумы совпадают.
     * @throws TankNotFoundException если один из аквариумов не найден.
     * @throws AnimalNotFoundException если животного нет в источнике.
     * @note Если приёмник не принял животное (например, IncompatibleSpeciesException или
     *       TankCapacityExceededException), оно возвращается в источник, а исключение пробрасывается.
     */
    void transferAnimal(int animalId, int fromTankId, int toTankId);
    /// @brief Общее число животных во всех аквариумах.
    int totalAnimals() const;
    /// @brief Число животных исчезающих видов.
    int endangeredAnimals() const;
    /// @brief Суточная потребность в корме всех животных, кг.
    double dailyFoodRequirement() const;
    /**
     * @brief Проводит кормления, назначенные на указанную минуту.
     * @param minute Минута от начала суток.
     * @param today Текущая дата.
     * @return Число накормленных животных (результат FeedingSchedule::execute).
     */
    int feedAnimals(int minute, const Date& today);
    /**
     * @brief Суточный цикл: жизнеобеспечение, голод, задачи на уборку.
     * @details При отказе оборудования создаётся задача ремонта (приоритет 5), для грязного
     *          аквариума — задача уборки, для изношенного фильтра — задача замены; затем
     *          со склада удаляется просроченный корм.
     * @param today Текущая дата.
     * @return Число созданных задач обслуживания.
     */
    int runDailyOperations(const Date& today);
    /**
     * @brief Выплачивает зарплату всем сотрудникам из бюджета.
     * @return Выплаченная сумма.
     * @throws BudgetExceededException если превышен лимит расходов.
     * @throws InsufficientFundsException если средств не хватает.
     */
    double paySalaries();
    /// @brief Краткая сводка: название, город, число аквариумов, животных и сотрудников.
    std::string summary() const;

private:
    std::string name_;                               ///< Название.
    std::string city_;                               ///< Город.
    std::vector<std::unique_ptr<Tank>> tanks_;       ///< Аквариумы (во владении).
    std::vector<std::unique_ptr<Employee>> employees_; ///< Сотрудники (во владении).
    std::vector<Exhibit> exhibits_;                  ///< Экспозиции.
    TicketOffice ticketOffice_;                      ///< Билетная касса.
    FoodStorage foodStorage_;                        ///< Склад корма.
    MaintenanceLog maintenance_;                     ///< Журнал обслуживания.
    StaffSchedule schedule_;                         ///< Расписание смен.
    FeedingSchedule feeding_;                        ///< Расписание кормлений.
    Budget budget_;                                  ///< Бюджет.
    Payroll payroll_;                                ///< Расчёт зарплаты.
};

}  // namespace ocean
