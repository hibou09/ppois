/**
 * @file Feeding.h
 * @brief Расписание кормлений и поставщики корма.
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Animals.h"
#include "Common.h"
#include "Food.h"

namespace ocean {

/// Запись расписания: какое животное, чем, сколько и когда кормить.
class FeedingEntry {
public:
    FeedingEntry(int animalId, std::string foodName, double amountKg, int minuteOfDay);

    int animalId() const { return animalId_; }
    const std::string& foodName() const { return foodName_; }
    double amountKg() const { return amountKg_; }
    int minuteOfDay() const { return minuteOfDay_; }

    /// Кормление «пора проводить», если до него не больше 15 минут.
    bool isDueAt(int minute) const;

private:
    int animalId_;
    std::string foodName_;
    double amountKg_;
    int minuteOfDay_;
};

/// Суточное расписание кормлений.
class FeedingSchedule {
public:
    FeedingSchedule();

    const std::vector<FeedingEntry>& entries() const { return entries_; }
    int executedCount() const { return executedCount_; }

    /// Бросает ScheduleConflictException, если кормления одного животного ближе часа.
    void addEntry(const FeedingEntry& entry);
    std::vector<FeedingEntry> entriesFor(int animalId) const;
    std::vector<FeedingEntry> dueAt(int minute) const;
    double dailyAmount(const std::string& foodName) const;
    /// Провести кормления, назначенные на minute; вернуть число накормленных.
    int execute(int minute, const std::vector<std::shared_ptr<Animal>>& animals,
                FoodStorage& storage, const Date& today);

private:
    std::vector<FeedingEntry> entries_;
    int executedCount_;
};

/// Поставщик корма.
class Supplier {
public:
    Supplier(std::string name, int deliveryDays, double discountPercent);

    const std::string& name() const { return name_; }
    int deliveryDays() const { return deliveryDays_; }
    double rating() const { return rating_; }

    double quote(double kg, double pricePerKg) const;
    Date deliveryDate(const Date& orderDate) const;
    /// Поставить партию корма; срок годности отсчитывается от даты доставки.
    FoodItem deliver(const std::string& foodName, double kg, double pricePerKg,
                     double caloriesPerKg, const Date& orderDate, int shelfLifeDays) const;
    /// Оценить поставщика по шкале 1..5; рейтинг — среднее оценок.
    void rate(int score);

private:
    std::string name_;
    int deliveryDays_;
    double discountPercent_;
    double rating_;
    int ratingsCount_;
};

}  // namespace ocean
