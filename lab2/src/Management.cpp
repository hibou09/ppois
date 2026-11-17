#include "Management.h"

#include <algorithm>
#include <utility>

#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- MaintenanceTask

MaintenanceTask::MaintenanceTask(int id, std::string description, Tank* tank, int priority,
                                 const Date& dueDate)
    : id_(id), description_(std::move(description)), tank_(tank), priority_(priority),
      dueDate_(dueDate), done_(false) {
    if (description_.empty()) {
        throw InvalidParameterException("task description must be non-empty");
    }
    if (priority < 1 || priority > 5) {
        throw InvalidParameterException("priority must be in [1, 5]");
    }
}

void MaintenanceTask::complete(const Employee& employee) {
    if (!employee.isActive()) {
        throw EmployeeNotQualifiedException(employee.name() + " is dismissed");
    }
    if (done_) {
        throw InvalidParameterException("task #" + std::to_string(id_) + " already completed");
    }
    done_ = true;
    completedBy_ = employee.name();
}

bool MaintenanceTask::isOverdue(const Date& today) const {
    return !done_ && dueDate_.isBefore(today);
}

void MaintenanceTask::escalate() {
    priority_ = std::min(5, priority_ + 1);
}

// ---------------------------------------------------------------- MaintenanceLog

MaintenanceLog::MaintenanceLog() : nextId_(1) {}

int MaintenanceLog::schedule(const std::string& description, Tank* tank, int priority,
                             const Date& dueDate) {
    tasks_.emplace_back(nextId_, description, tank, priority, dueDate);
    return nextId_++;
}

MaintenanceTask& MaintenanceLog::find(int taskId) {
    for (auto& t : tasks_) {
        if (t.id() == taskId) {
            return t;
        }
    }
    throw TaskNotFoundException(taskId);
}

void MaintenanceLog::complete(int taskId, const Employee& employee) {
    find(taskId).complete(employee);
}

int MaintenanceLog::pendingCount() const {
    return static_cast<int>(std::count_if(tasks_.begin(), tasks_.end(),
                                          [](const MaintenanceTask& t) { return !t.isDone(); }));
}

int MaintenanceLog::overdueCount(const Date& today) const {
    return static_cast<int>(std::count_if(
        tasks_.begin(), tasks_.end(),
        [&today](const MaintenanceTask& t) { return t.isOverdue(today); }));
}

int MaintenanceLog::highPriorityPending() const {
    return static_cast<int>(
        std::count_if(tasks_.begin(), tasks_.end(), [](const MaintenanceTask& t) {
            return !t.isDone() && t.priority() >= 4;
        }));
}

int MaintenanceLog::tasksForTank(int tankId) const {
    return static_cast<int>(
        std::count_if(tasks_.begin(), tasks_.end(), [tankId](const MaintenanceTask& t) {
            return t.tank() != nullptr && t.tank()->id() == tankId;
        }));
}

// ---------------------------------------------------------------- Budget

Budget::Budget(double initialBalance, double expenseLimit)
    : balance_(initialBalance), income_(0.0), expenses_(0.0), expenseLimit_(expenseLimit) {
    if (initialBalance < 0.0 || expenseLimit <= 0.0) {
        throw InvalidParameterException("invalid budget values");
    }
}

void Budget::addIncome(double amount) {
    if (amount <= 0.0) {
        throw InvalidParameterException("income must be positive");
    }
    income_ += amount;
    balance_ += amount;
}

void Budget::spend(const std::string& category, double amount) {
    if (category.empty() || amount <= 0.0) {
        throw InvalidParameterException("expense needs category and positive amount");
    }
    if (expenses_ + amount > expenseLimit_) {
        throw BudgetExceededException(category + ": limit " + std::to_string(expenseLimit_));
    }
    if (amount > balance_) {
        throw InsufficientFundsException(amount, balance_);
    }
    balance_ -= amount;
    expenses_ += amount;
    byCategory_[category] += amount;
}

double Budget::spentOn(const std::string& category) const {
    auto it = byCategory_.find(category);
    return it == byCategory_.end() ? 0.0 : it->second;
}

double Budget::profit() const {
    return income_ - expenses_;
}

double Budget::remainingLimit() const {
    return expenseLimit_ - expenses_;
}

// ---------------------------------------------------------------- Payroll

Payroll::Payroll(double bonusPercent) : totalPaid_(0.0), paymentsCount_(0), bonusPercent_(0.0) {
    setBonus(bonusPercent);
}

void Payroll::setBonus(double percent) {
    if (percent < 0.0 || percent > 100.0) {
        throw InvalidParameterException("bonus must be in [0, 100]");
    }
    bonusPercent_ = percent;
}

double Payroll::calculate(const Employee& employee) const {
    return employee.monthlySalary() * (1.0 + bonusPercent_ / 100.0);
}

double Payroll::payAll(const std::vector<Employee*>& employees, Budget& budget) {
    double total = 0.0;
    for (const Employee* e : employees) {
        if (e != nullptr && e->isActive()) {
            total += calculate(*e);
        }
    }
    if (total <= 0.0) {
        return 0.0;
    }
    budget.spend("salary", total);  // всё или ничего
    for (const Employee* e : employees) {
        if (e != nullptr && e->isActive()) {
            ++paymentsCount_;
        }
    }
    totalPaid_ += total;
    return total;
}

// ---------------------------------------------------------------- Oceanarium

Oceanarium::Oceanarium(std::string name, std::string city, double initialBalance,
                       double ticketPrice)
    : name_(std::move(name)), city_(std::move(city)), ticketOffice_(ticketPrice, 2000),
      foodStorage_(5000.0), budget_(initialBalance, initialBalance * 10.0) {
    if (name_.empty()) {
        throw InvalidParameterException("oceanarium name must be non-empty");
    }
}

Tank& Oceanarium::addTank(std::unique_ptr<Tank> tank) {
    if (!tank) {
        throw InvalidParameterException("tank must not be null");
    }
    for (const auto& t : tanks_) {
        if (t->id() == tank->id()) {
            throw InvalidParameterException("tank #" + std::to_string(tank->id()) + " exists");
        }
    }
    tanks_.push_back(std::move(tank));
    return *tanks_.back();
}

Tank& Oceanarium::findTank(int tankId) {
    for (auto& t : tanks_) {
        if (t->id() == tankId) {
            return *t;
        }
    }
    throw TankNotFoundException(tankId);
}

int Oceanarium::tankCount() const {
    return static_cast<int>(tanks_.size());
}

Employee& Oceanarium::hire(std::unique_ptr<Employee> employee) {
    if (!employee) {
        throw InvalidParameterException("employee must not be null");
    }
    for (const auto& e : employees_) {
        if (e->id() == employee->id()) {
            throw InvalidParameterException("employee #" + std::to_string(employee->id()) +
                                            " exists");
        }
    }
    employees_.push_back(std::move(employee));
    return *employees_.back();
}

Employee& Oceanarium::findEmployee(int employeeId) {
    for (auto& e : employees_) {
        if (e->id() == employeeId) {
            return *e;
        }
    }
    throw EmployeeNotFoundException(employeeId);
}

void Oceanarium::fire(int employeeId) {
    findEmployee(employeeId).dismiss();
    schedule_.removeShiftsOf(employeeId);
}

Exhibit& Oceanarium::addExhibit(const Exhibit& exhibit) {
    exhibits_.push_back(exhibit);
    return exhibits_.back();
}

Tank& Oceanarium::locateAnimal(int animalId) {
    for (auto& t : tanks_) {
        if (t->hasAnimal(animalId)) {
            return *t;
        }
    }
    throw AnimalNotFoundException(animalId);
}

void Oceanarium::transferAnimal(int animalId, int fromTankId, int toTankId) {
    if (fromTankId == toTankId) {
        throw InvalidParameterException("source and destination tanks are the same");
    }
    Tank& from = findTank(fromTankId);
    Tank& to = findTank(toTankId);
    auto animal = from.removeAnimal(animalId);
    try {
        to.addAnimal(animal);
    } catch (...) {
        from.addAnimal(animal);  // откат
        throw;
    }
}

int Oceanarium::totalAnimals() const {
    int total = 0;
    for (const auto& t : tanks_) {
        total += t->animalCount();
    }
    return total;
}

int Oceanarium::endangeredAnimals() const {
    int total = 0;
    for (const auto& t : tanks_) {
        for (const auto& a : t->animals()) {
            if (a->species().isEndangered()) {
                ++total;
            }
        }
    }
    return total;
}

double Oceanarium::dailyFoodRequirement() const {
    double total = 0.0;
    for (const auto& t : tanks_) {
        total += t->dailyFoodRequirement();
    }
    return total;
}

int Oceanarium::feedAnimals(int minute, const Date& today) {
    std::vector<std::shared_ptr<Animal>> all;
    for (const auto& t : tanks_) {
        all.insert(all.end(), t->animals().begin(), t->animals().end());
    }
    return feeding_.execute(minute, all, foodStorage_, today);
}

int Oceanarium::runDailyOperations(const Date& today) {
    int created = 0;
    for (auto& t : tanks_) {
        try {
            t->runLifeSupport(24.0);
        } catch (const EquipmentFailureException& e) {
            maintenance_.schedule(std::string("Repair: ") + e.what(), t.get(), 5, today);
            ++created;
        }
        for (const auto& a : t->animals()) {
            a->passDay();
        }
        if (t->needsCleaning()) {
            maintenance_.schedule("Clean " + t->name(), t.get(), 4, today.addDays(1));
            ++created;
        }
        if (t->filtration().needsMaintenance()) {
            maintenance_.schedule("Replace filter in " + t->name(), t.get(), 3, today.addDays(2));
            ++created;
        }
    }
    foodStorage_.removeExpired(today);
    return created;
}

double Oceanarium::paySalaries() {
    std::vector<Employee*> staff;
    for (auto& e : employees_) {
        staff.push_back(e.get());
    }
    return payroll_.payAll(staff, budget_);
}

std::string Oceanarium::summary() const {
    return name_ + " (" + city_ + "): " + std::to_string(tanks_.size()) + " tanks, " +
           std::to_string(totalAnimals()) + " animals, " + std::to_string(employees_.size()) +
           " employees";
}

}  // namespace ocean
