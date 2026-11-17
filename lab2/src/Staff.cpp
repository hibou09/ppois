#include "Staff.h"

#include <algorithm>
#include <utility>

#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- Employee

Employee::Employee(int id, std::string name, double baseSalary, const Date& hireDate)
    : id_(id), name_(std::move(name)), baseSalary_(baseSalary), hireDate_(hireDate),
      active_(true) {
    if (id <= 0 || name_.empty()) {
        throw InvalidParameterException("employee needs positive id and name");
    }
    if (baseSalary <= 0.0) {
        throw InvalidParameterException("salary must be positive");
    }
}

double Employee::monthlySalary() const {
    return baseSalary_;
}

int Employee::yearsOfService(const Date& today) const {
    return std::max(0, hireDate_.daysUntil(today) / 365);
}

void Employee::giveRaise(double percent) {
    if (percent <= 0.0 || percent > 50.0) {
        throw InvalidParameterException("raise must be in (0, 50] percent");
    }
    baseSalary_ *= (1.0 + percent / 100.0);
}

void Employee::dismiss() {
    active_ = false;
}

// ---------------------------------------------------------------- Aquarist

Aquarist::Aquarist(int id, std::string name, double baseSalary, const Date& hireDate,
                   int certLevel)
    : Employee(id, std::move(name), baseSalary, hireDate), certLevel_(certLevel),
      inspections_(0) {
    if (certLevel < 1 || certLevel > 3) {
        throw InvalidParameterException("certification level must be in [1, 3]");
    }
}

double Aquarist::monthlySalary() const {
    return baseSalary_ * (1.0 + 0.1 * (certLevel_ - 1));
}

void Aquarist::assignTank(Tank* tank) {
    if (tank == nullptr) {
        throw InvalidParameterException("tank must not be null");
    }
    if (isResponsibleFor(tank->id())) {
        throw InvalidParameterException("tank already assigned to " + name_);
    }
    if (static_cast<int>(assignedTanks_.size()) >= certLevel_ * 2) {
        throw EmployeeNotQualifiedException(name_ + " cannot maintain more than " +
                                            std::to_string(certLevel_ * 2) + " tanks");
    }
    if (dynamic_cast<CoralReefTank*>(tank) != nullptr && certLevel_ < 2) {
        throw EmployeeNotQualifiedException("coral reef tanks require level 2 certification");
    }
    assignedTanks_.push_back(tank);
}

bool Aquarist::isResponsibleFor(int tankId) const {
    return std::any_of(assignedTanks_.begin(), assignedTanks_.end(),
                       [tankId](const Tank* t) { return t->id() == tankId; });
}

bool Aquarist::inspect(const Tank& tank) {
    ++inspections_;
    return tank.isHealthy() && !tank.needsCleaning() && tank.water().qualityIndex() >= 70.0;
}

void Aquarist::cleanTank(Tank& tank) {
    if (!active_) {
        throw EmployeeNotQualifiedException(name_ + " is dismissed");
    }
    if (!isResponsibleFor(tank.id())) {
        throw EmployeeNotQualifiedException(name_ + " is not responsible for " + tank.name());
    }
    tank.performWaterChange(0.3);
    if (tank.filtration().needsMaintenance()) {
        tank.filtration().replaceFilter();
    }
}

bool Aquarist::promote() {
    if (certLevel_ >= 3) {
        return false;
    }
    ++certLevel_;
    return true;
}

// ---------------------------------------------------------------- Veterinarian

Veterinarian::Veterinarian(int id, std::string name, double baseSalary, const Date& hireDate,
                           std::string licenseNumber, const Date& licenseExpiry,
                           std::string specialization)
    : Employee(id, std::move(name), baseSalary, hireDate),
      licenseNumber_(std::move(licenseNumber)), licenseExpiry_(licenseExpiry),
      specialization_(std::move(specialization)), examinations_(0) {
    if (licenseNumber_.empty()) {
        throw InvalidParameterException("license number must be non-empty");
    }
}

double Veterinarian::monthlySalary() const {
    return baseSalary_ * 1.3;
}

bool Veterinarian::isLicenseValid(const Date& today) const {
    return !licenseExpiry_.isBefore(today);
}

void Veterinarian::renewLicense(const Date& newExpiry) {
    if (!licenseExpiry_.isBefore(newExpiry)) {
        throw InvalidDateException("new license expiry must be later than current");
    }
    licenseExpiry_ = newExpiry;
}

void Veterinarian::ensureLicense(const Date& today) const {
    if (!active_) {
        throw EmployeeNotQualifiedException(name_ + " is dismissed");
    }
    if (!isLicenseValid(today)) {
        throw LicenseExpiredException(name_ + ", license " + licenseNumber_ + " expired on " +
                                      licenseExpiry_.toString());
    }
}

void Veterinarian::examine(Animal& animal, const Date& today, const std::string& diagnosis,
                           int severity) {
    ensureLicense(today);
    animal.health().addEntry(MedicalEntry(today, diagnosis, "observation", name_, severity));
    ++examinations_;
}

double Veterinarian::treat(Animal& animal, Medication& medication, const Date& today) {
    ensureLicense(today);
    double dose = medication.dispense(animal, today);
    animal.health().addEntry(MedicalEntry(today, "treated",
                                          medication.name() + " " + std::to_string(dose) + " mg",
                                          name_, 1));
    return dose;
}

// ---------------------------------------------------------------- Diver

Diver::Diver(int id, std::string name, double baseSalary, const Date& hireDate, double maxDepthM)
    : Employee(id, std::move(name), baseSalary, hireDate), maxDepthM_(maxDepthM),
      diveHours_(0.0) {
    if (maxDepthM <= 0.0) {
        throw InvalidParameterException("max depth must be positive");
    }
}

double Diver::monthlySalary() const {
    return baseSalary_ * 1.2;
}

bool Diver::canDiveIn(const OpenOceanTank& tank) const {
    return tank.depth() <= maxDepthM_;
}

void Diver::dive(OpenOceanTank& tank, double hours) {
    if (hours <= 0.0 || hours > 4.0) {
        throw InvalidParameterException("dive duration must be in (0, 4] hours");
    }
    if (!canDiveIn(tank)) {
        throw EmployeeNotQualifiedException(name_ + " is certified only to " +
                                            std::to_string(maxDepthM_) + " m");
    }
    diveHours_ += hours;
}

void Diver::upgradeCertification(double newMaxDepthM) {
    if (newMaxDepthM <= maxDepthM_) {
        throw InvalidParameterException("new certification depth must be greater");
    }
    maxDepthM_ = newMaxDepthM;
}

// ---------------------------------------------------------------- Trainer

Trainer::Trainer(int id, std::string name, double baseSalary, const Date& hireDate,
                 std::string specialty)
    : Employee(id, std::move(name), baseSalary, hireDate), specialty_(std::move(specialty)),
      showsConducted_(0) {}

bool Trainer::train(Seal& seal, const std::string& trick) {
    return seal.learnTrick(trick);
}

int Trainer::conductShow(Show& show) {
    if (show.isCancelled()) {
        throw InvalidParameterException("show " + show.title() + " is cancelled");
    }
    if (show.performers().empty()) {
        throw InvalidParameterException("show " + show.title() + " has no performers");
    }
    int performed = 0;
    for (Seal* seal : show.performers()) {
        for (const auto& trick : seal->tricks()) {
            if (seal->perform(trick)) {
                ++performed;
            }
        }
    }
    ++showsConducted_;
    return performed;
}

// ---------------------------------------------------------------- Guide

Guide::Guide(int id, std::string name, double baseSalary, const Date& hireDate,
             std::vector<std::string> languages)
    : Employee(id, std::move(name), baseSalary, hireDate), languages_(std::move(languages)),
      toursGiven_(0) {
    if (languages_.empty()) {
        throw InvalidParameterException("guide must speak at least one language");
    }
}

double Guide::monthlySalary() const {
    // +5% за каждый язык сверх первого.
    return baseSalary_ * (1.0 + 0.05 * (static_cast<double>(languages_.size()) - 1.0));
}

bool Guide::speaks(const std::string& language) const {
    return std::find(languages_.begin(), languages_.end(), language) != languages_.end();
}

void Guide::learnLanguage(const std::string& language) {
    if (language.empty()) {
        throw InvalidParameterException("language must be non-empty");
    }
    if (!speaks(language)) {
        languages_.push_back(language);
    }
}

int Guide::conductTour(Tour& tour) {
    if (tour.guide() != this) {
        throw EmployeeNotQualifiedException(name_ + " is not assigned to tour #" +
                                            std::to_string(tour.id()));
    }
    tour.complete();
    ++toursGiven_;
    return tour.groupSize();
}

// ---------------------------------------------------------------- Cashier

Cashier::Cashier(int id, std::string name, double baseSalary, const Date& hireDate,
                 int registerId)
    : Employee(id, std::move(name), baseSalary, hireDate), registerId_(registerId),
      shiftRevenue_(0.0), ticketsSold_(0) {}

Ticket Cashier::sellTicket(TicketOffice& office, Visitor& visitor, TicketType type,
                           const Date& date, const Membership* membership, PromoCode* promo) {
    if (!active_) {
        throw EmployeeNotQualifiedException(name_ + " is dismissed");
    }
    Ticket ticket = office.sell(visitor, type, date, membership, promo);
    shiftRevenue_ += ticket.price();
    ++ticketsSold_;
    return ticket;
}

double Cashier::closeShift() {
    double revenue = shiftRevenue_;
    shiftRevenue_ = 0.0;
    ticketsSold_ = 0;
    return revenue;
}

// ---------------------------------------------------------------- Shift

Shift::Shift(int employeeId, const Date& date, const TimeSlot& slot)
    : employeeId_(employeeId), date_(date), slot_(slot) {}

bool Shift::overlaps(const Shift& other) const {
    return employeeId_ == other.employeeId_ && date_ == other.date_ && slot_.overlaps(other.slot_);
}

double Shift::hours() const {
    return slot_.durationMinutes() / 60.0;
}

// ---------------------------------------------------------------- StaffSchedule

void StaffSchedule::addShift(const Shift& shift) {
    for (const auto& s : shifts_) {
        if (s.overlaps(shift)) {
            throw ScheduleConflictException("employee #" + std::to_string(shift.employeeId()) +
                                            " already works " + s.slot().toString() + " on " +
                                            s.date().toString());
        }
    }
    shifts_.push_back(shift);
}

double StaffSchedule::totalHours(int employeeId) const {
    double total = 0.0;
    for (const auto& s : shifts_) {
        if (s.employeeId() == employeeId) {
            total += s.hours();
        }
    }
    return total;
}

std::vector<Shift> StaffSchedule::shiftsOn(const Date& date) const {
    std::vector<Shift> result;
    for (const auto& s : shifts_) {
        if (s.date() == date) {
            result.push_back(s);
        }
    }
    return result;
}

bool StaffSchedule::isWorking(int employeeId, const Date& date, int minute) const {
    return std::any_of(shifts_.begin(), shifts_.end(), [&](const Shift& s) {
        return s.employeeId() == employeeId && s.date() == date && s.slot().contains(minute);
    });
}

int StaffSchedule::removeShiftsOf(int employeeId) {
    auto before = shifts_.size();
    shifts_.erase(std::remove_if(shifts_.begin(), shifts_.end(),
                                 [employeeId](const Shift& s) {
                                     return s.employeeId() == employeeId;
                                 }),
                  shifts_.end());
    return static_cast<int>(before - shifts_.size());
}

}  // namespace ocean
