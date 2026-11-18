/**
 * @file Food.h
 * @brief Корм и склад кормов.
 */
#pragma once

#include <string>
#include <vector>

#include "Common.h"

namespace ocean {

/**
 * @brief Партия корма определённого вида.
 */
class FoodItem {
public:
    /**
     * @brief Создаёт партию корма.
     * @param name Название корма.
     * @param caloriesPerKg Калорийность, ккал на кг (> 0).
     * @param stockKg Остаток, кг (>= 0).
     * @param pricePerKg Цена за кг (>= 0).
     * @param expiry Дата окончания срока годности.
     * @throws InvalidParameterException если название пусто или числовые значения некорректны.
     */
    FoodItem(std::string name, double caloriesPerKg, double stockKg, double pricePerKg,
             const Date& expiry);

    /// @brief Название корма.
    const std::string& name() const { return name_; }
    /// @brief Калорийность, ккал на кг.
    double caloriesPerKg() const { return caloriesPerKg_; }
    /// @brief Остаток на складе, кг.
    double stockKg() const { return stockKg_; }
    /// @brief Цена за килограмм.
    double pricePerKg() const { return pricePerKg_; }
    /// @brief Дата окончания срока годности.
    const Date& expiry() const { return expiry_; }

    /**
     * @brief Списывает корм; проверяет срок годности и остаток.
     * @param kg Количество, кг (> 0).
     * @param today Текущая дата.
     * @throws InvalidParameterException если kg <= 0.
     * @throws ExpiredProductException если срок годности истёк.
     * @throws InsufficientFoodException если остатка меньше запрошенного.
     */
    void consume(double kg, const Date& today);
    /**
     * @brief Проверяет, просрочена ли партия.
     * @param today Текущая дата.
     * @return true, если срок годности закончился раньше today.
     */
    bool isExpired(const Date& today) const;
    /**
     * @brief Сколько дней осталось до конца срока годности.
     * @param today Текущая дата.
     * @return Число дней (отрицательное, если срок истёк).
     */
    int daysUntilExpiry(const Date& today) const;
    /**
     * @brief Стоимость заданного количества корма.
     * @param kg Количество, кг (>= 0).
     * @return kg * цена за кг.
     * @throws InvalidParameterException если kg < 0.
     */
    double cost(double kg) const;
    /**
     * @brief Увеличивает остаток.
     * @param kg Добавляемое количество, кг (> 0).
     * @throws InvalidParameterException если kg <= 0.
     */
    void addStock(double kg);

private:
    std::string name_;      ///< Название корма.
    double caloriesPerKg_;  ///< Калорийность, ккал на кг.
    double stockKg_;        ///< Остаток, кг.
    double pricePerKg_;     ///< Цена за кг.
    Date expiry_;           ///< Дата окончания срока годности.
};

/**
 * @brief Склад кормов с ограниченной вместимостью.
 */
class FoodStorage {
public:
    /**
     * @brief Создаёт склад.
     * @param capacityKg Вместимость, кг (> 0).
     * @throws InvalidParameterException если вместимость не положительна.
     */
    explicit FoodStorage(double capacityKg);

    /// @brief Вместимость склада, кг.
    double capacityKg() const { return capacityKg_; }
    /// @brief Все партии на складе.
    const std::vector<FoodItem>& items() const { return items_; }

    /**
     * @brief Добавляет партию; одноимённые партии объединяются.
     * @param item Добавляемая партия.
     * @throws StorageFullException если свободного места не хватает.
     */
    void addItem(const FoodItem& item);
    /**
     * @brief Проверяет наличие корма с данным названием.
     * @param name Название корма.
     * @return true, если такая партия есть на складе.
     */
    bool contains(const std::string& name) const;
    /**
     * @brief Находит партию по названию.
     * @param name Название корма.
     * @return Ссылка на найденную партию.
     * @throws InsufficientFoodException если корма с таким названием нет.
     */
    FoodItem& find(const std::string& name);
    /**
     * @brief Забирает корм со склада.
     * @param name Название корма.
     * @param kg Количество, кг.
     * @param today Текущая дата.
     * @throws InsufficientFoodException если корма нет или его не хватает.
     * @throws ExpiredProductException если партия просрочена.
     * @throws InvalidParameterException если kg <= 0.
     */
    void take(const std::string& name, double kg, const Date& today);
    /**
     * @brief Удаляет просроченные партии.
     * @param today Текущая дата.
     * @return Количество удалённых партий.
     */
    int removeExpired(const Date& today);
    /**
     * @brief Суммарный остаток всех партий.
     * @return Масса, кг.
     */
    double totalStockKg() const;
    /**
     * @brief Свободная вместимость склада.
     * @return Масса, кг.
     */
    double freeCapacityKg() const;
    /**
     * @brief Суммарная стоимость запасов.
     * @return Стоимость всех партий.
     */
    double totalValue() const;

private:
    double capacityKg_;           ///< Вместимость, кг.
    std::vector<FoodItem> items_; ///< Партии на складе.
};

}  // namespace ocean
