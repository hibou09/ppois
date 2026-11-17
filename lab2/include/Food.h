/**
 * @file Food.h
 * @brief Корм и склад кормов.
 */
#pragma once

#include <string>
#include <vector>

#include "Common.h"

namespace ocean {

/// Партия корма определённого вида.
class FoodItem {
public:
    FoodItem(std::string name, double caloriesPerKg, double stockKg, double pricePerKg,
             const Date& expiry);

    const std::string& name() const { return name_; }
    double caloriesPerKg() const { return caloriesPerKg_; }
    double stockKg() const { return stockKg_; }
    double pricePerKg() const { return pricePerKg_; }
    const Date& expiry() const { return expiry_; }

    /// Списать kg килограммов корма; проверяет срок годности и остаток.
    void consume(double kg, const Date& today);
    bool isExpired(const Date& today) const;
    int daysUntilExpiry(const Date& today) const;
    double cost(double kg) const;
    void addStock(double kg);

private:
    std::string name_;
    double caloriesPerKg_;
    double stockKg_;
    double pricePerKg_;
    Date expiry_;
};

/// Склад кормов с ограниченной вместимостью.
class FoodStorage {
public:
    explicit FoodStorage(double capacityKg);

    double capacityKg() const { return capacityKg_; }
    const std::vector<FoodItem>& items() const { return items_; }

    /// Добавить партию; одноимённые партии объединяются.
    void addItem(const FoodItem& item);
    bool contains(const std::string& name) const;
    FoodItem& find(const std::string& name);
    void take(const std::string& name, double kg, const Date& today);
    /// Удалить просроченные партии, вернуть их количество.
    int removeExpired(const Date& today);
    double totalStockKg() const;
    double freeCapacityKg() const;
    double totalValue() const;

private:
    double capacityKg_;
    std::vector<FoodItem> items_;
};

}  // namespace ocean
