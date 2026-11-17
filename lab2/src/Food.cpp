#include "Food.h"

#include <algorithm>
#include <utility>

#include "Exceptions.h"

namespace ocean {

// ---------------------------------------------------------------- FoodItem

FoodItem::FoodItem(std::string name, double caloriesPerKg, double stockKg, double pricePerKg,
                   const Date& expiry)
    : name_(std::move(name)), caloriesPerKg_(caloriesPerKg), stockKg_(stockKg),
      pricePerKg_(pricePerKg), expiry_(expiry) {
    if (name_.empty()) {
        throw InvalidParameterException("food name must be non-empty");
    }
    if (caloriesPerKg <= 0.0 || stockKg < 0.0 || pricePerKg < 0.0) {
        throw InvalidParameterException("invalid food item values");
    }
}

void FoodItem::consume(double kg, const Date& today) {
    if (kg <= 0.0) {
        throw InvalidParameterException("amount of food must be positive");
    }
    if (isExpired(today)) {
        throw ExpiredProductException(name_ + " expired on " + expiry_.toString());
    }
    if (kg > stockKg_) {
        throw InsufficientFoodException(name_ + ": requested " + std::to_string(kg) +
                                        " kg, available " + std::to_string(stockKg_) + " kg");
    }
    stockKg_ -= kg;
}

bool FoodItem::isExpired(const Date& today) const {
    return expiry_.isBefore(today);
}

int FoodItem::daysUntilExpiry(const Date& today) const {
    return today.daysUntil(expiry_);
}

double FoodItem::cost(double kg) const {
    if (kg < 0.0) {
        throw InvalidParameterException("amount must be non-negative");
    }
    return kg * pricePerKg_;
}

void FoodItem::addStock(double kg) {
    if (kg <= 0.0) {
        throw InvalidParameterException("added stock must be positive");
    }
    stockKg_ += kg;
}

// ---------------------------------------------------------------- FoodStorage

FoodStorage::FoodStorage(double capacityKg) : capacityKg_(capacityKg) {
    if (capacityKg <= 0.0) {
        throw InvalidParameterException("storage capacity must be positive");
    }
}

void FoodStorage::addItem(const FoodItem& item) {
    if (item.stockKg() > freeCapacityKg()) {
        throw StorageFullException("cannot store " + std::to_string(item.stockKg()) + " kg of " +
                                   item.name());
    }
    for (auto& existing : items_) {
        if (existing.name() == item.name()) {
            if (item.stockKg() > 0.0) {
                existing.addStock(item.stockKg());
            }
            return;
        }
    }
    items_.push_back(item);
}

bool FoodStorage::contains(const std::string& name) const {
    return std::any_of(items_.begin(), items_.end(),
                       [&name](const FoodItem& f) { return f.name() == name; });
}

FoodItem& FoodStorage::find(const std::string& name) {
    for (auto& item : items_) {
        if (item.name() == name) {
            return item;
        }
    }
    throw InsufficientFoodException("no food named " + name + " in storage");
}

void FoodStorage::take(const std::string& name, double kg, const Date& today) {
    find(name).consume(kg, today);
}

int FoodStorage::removeExpired(const Date& today) {
    auto before = items_.size();
    items_.erase(std::remove_if(items_.begin(), items_.end(),
                                [&today](const FoodItem& f) { return f.isExpired(today); }),
                 items_.end());
    return static_cast<int>(before - items_.size());
}

double FoodStorage::totalStockKg() const {
    double total = 0.0;
    for (const auto& item : items_) {
        total += item.stockKg();
    }
    return total;
}

double FoodStorage::freeCapacityKg() const {
    return capacityKg_ - totalStockKg();
}

double FoodStorage::totalValue() const {
    double total = 0.0;
    for (const auto& item : items_) {
        total += item.cost(item.stockKg());
    }
    return total;
}

}  // namespace ocean
