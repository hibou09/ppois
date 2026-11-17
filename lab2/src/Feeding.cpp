#include "Feeding.h"

#include <cstdlib>
#include <utility>

#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- FeedingEntry

FeedingEntry::FeedingEntry(int animalId, std::string foodName, double amountKg, int minuteOfDay)
    : animalId_(animalId), foodName_(std::move(foodName)), amountKg_(amountKg),
      minuteOfDay_(minuteOfDay) {
    if (foodName_.empty() || amountKg <= 0.0) {
        throw InvalidParameterException("feeding entry needs food name and positive amount");
    }
    if (minuteOfDay < 0 || minuteOfDay >= 24 * 60) {
        throw InvalidParameterException("minute of day must be in [0, 1440)");
    }
}

bool FeedingEntry::isDueAt(int minute) const {
    return std::abs(minute - minuteOfDay_) <= 15;
}

// ---------------------------------------------------------------- FeedingSchedule

FeedingSchedule::FeedingSchedule() : executedCount_(0) {}

void FeedingSchedule::addEntry(const FeedingEntry& entry) {
    for (const auto& e : entries_) {
        if (e.animalId() == entry.animalId() &&
            std::abs(e.minuteOfDay() - entry.minuteOfDay()) < 60) {
            throw ScheduleConflictException("animal #" + std::to_string(entry.animalId()) +
                                            " is already fed within an hour of that time");
        }
    }
    entries_.push_back(entry);
}

std::vector<FeedingEntry> FeedingSchedule::entriesFor(int animalId) const {
    std::vector<FeedingEntry> result;
    for (const auto& e : entries_) {
        if (e.animalId() == animalId) {
            result.push_back(e);
        }
    }
    return result;
}

std::vector<FeedingEntry> FeedingSchedule::dueAt(int minute) const {
    std::vector<FeedingEntry> result;
    for (const auto& e : entries_) {
        if (e.isDueAt(minute)) {
            result.push_back(e);
        }
    }
    return result;
}

double FeedingSchedule::dailyAmount(const std::string& foodName) const {
    double total = 0.0;
    for (const auto& e : entries_) {
        if (e.foodName() == foodName) {
            total += e.amountKg();
        }
    }
    return total;
}

int FeedingSchedule::execute(int minute, const std::vector<std::shared_ptr<Animal>>& animals,
                             FoodStorage& storage, const Date& today) {
    int fed = 0;
    for (const auto& entry : dueAt(minute)) {
        for (const auto& animal : animals) {
            if (animal && animal->id() == entry.animalId()) {
                animal->feed(storage.find(entry.foodName()), entry.amountKg(), today);
                ++fed;
                ++executedCount_;
            }
        }
    }
    return fed;
}

// ---------------------------------------------------------------- Supplier

Supplier::Supplier(std::string name, int deliveryDays, double discountPercent)
    : name_(std::move(name)), deliveryDays_(deliveryDays), discountPercent_(discountPercent),
      rating_(0.0), ratingsCount_(0) {
    if (name_.empty() || deliveryDays < 0 || discountPercent < 0.0 || discountPercent >= 100.0) {
        throw InvalidParameterException("invalid supplier values");
    }
}

double Supplier::quote(double kg, double pricePerKg) const {
    if (kg <= 0.0 || pricePerKg < 0.0) {
        throw InvalidParameterException("quote needs positive amount and non-negative price");
    }
    return kg * pricePerKg * (1.0 - discountPercent_ / 100.0);
}

Date Supplier::deliveryDate(const Date& orderDate) const {
    return orderDate.addDays(deliveryDays_);
}

FoodItem Supplier::deliver(const std::string& foodName, double kg, double pricePerKg,
                           double caloriesPerKg, const Date& orderDate, int shelfLifeDays) const {
    if (shelfLifeDays <= 0) {
        throw InvalidParameterException("shelf life must be positive");
    }
    double unitPrice = quote(kg, pricePerKg) / kg;
    return FoodItem(foodName, caloriesPerKg, kg, unitPrice,
                    deliveryDate(orderDate).addDays(shelfLifeDays));
}

void Supplier::rate(int score) {
    if (score < 1 || score > 5) {
        throw InvalidParameterException("rating must be in [1, 5]");
    }
    rating_ = (rating_ * ratingsCount_ + score) / (ratingsCount_ + 1);
    ++ratingsCount_;
}

}  // namespace ocean
