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

/**
 * @brief Запись расписания: какое животное, чем, сколько и когда кормить.
 */
class FeedingEntry {
public:
    /**
     * @brief Создаёт запись о кормлении.
     * @param animalId Идентификатор животного.
     * @param foodName Название корма (непустое).
     * @param amountKg Количество корма, кг (положительное).
     * @param minuteOfDay Время кормления в минутах от начала суток (0..1439).
     * @throws InvalidParameterException если название пусто, количество не положительно
     *         или время вне диапазона [0, 1440).
     */
    FeedingEntry(int animalId, std::string foodName, double amountKg, int minuteOfDay);

    /// @brief Идентификатор животного.
    int animalId() const { return animalId_; }
    /// @brief Название корма.
    const std::string& foodName() const { return foodName_; }
    /// @brief Количество корма, кг.
    double amountKg() const { return amountKg_; }
    /// @brief Время кормления, минута суток.
    int minuteOfDay() const { return minuteOfDay_; }

    /**
     * @brief Кормление «пора проводить», если до него не больше 15 минут.
     * @param minute Текущая минута суток.
     * @return true, если |minute - minuteOfDay| не превышает 15.
     */
    bool isDueAt(int minute) const;

private:
    int animalId_;          ///< Идентификатор животного.
    std::string foodName_;  ///< Название корма.
    double amountKg_;       ///< Количество корма, кг.
    int minuteOfDay_;       ///< Время кормления, минута суток.
};

/**
 * @brief Суточное расписание кормлений.
 */
class FeedingSchedule {
public:
    /// @brief Создаёт пустое расписание.
    FeedingSchedule();

    /// @brief Все записи расписания.
    const std::vector<FeedingEntry>& entries() const { return entries_; }
    /// @brief Сколько кормлений проведено с момента создания расписания.
    int executedCount() const { return executedCount_; }

    /**
     * @brief Добавляет запись в расписание.
     * @param entry Новая запись.
     * @throws ScheduleConflictException если кормления одного животного ближе часа.
     */
    void addEntry(const FeedingEntry& entry);
    /**
     * @brief Записи для одного животного.
     * @param animalId Идентификатор животного.
     * @return Список его записей.
     */
    std::vector<FeedingEntry> entriesFor(int animalId) const;
    /**
     * @brief Записи, которые пора выполнять.
     * @param minute Текущая минута суток.
     * @return Записи, для которых isDueAt(minute) истинно.
     */
    std::vector<FeedingEntry> dueAt(int minute) const;
    /**
     * @brief Суточное количество корма.
     * @param foodName Название корма.
     * @return Суммарное количество этого корма во всех записях, кг.
     */
    double dailyAmount(const std::string& foodName) const;
    /**
     * @brief Проводит кормления, назначенные на minute.
     * @details Исключения, выбрасываемые при поиске корма на складе и кормлении животного,
     *          передаются вызывающему коду.
     * @param minute Текущая минута суток.
     * @param animals Животные, которых можно кормить.
     * @param storage Склад корма.
     * @param today Текущая дата.
     * @return Число накормленных животных.
     */
    int execute(int minute, const std::vector<std::shared_ptr<Animal>>& animals,
                FoodStorage& storage, const Date& today);

private:
    std::vector<FeedingEntry> entries_;  ///< Записи расписания.
    int executedCount_;                  ///< Счётчик проведённых кормлений.
};

/**
 * @brief Поставщик корма.
 */
class Supplier {
public:
    /**
     * @brief Создаёт поставщика.
     * @param name Название (непустое).
     * @param deliveryDays Срок доставки, дней (неотрицательный).
     * @param discountPercent Скидка, % (от 0 до 100, не включая 100).
     * @throws InvalidParameterException если название пусто, срок отрицателен или скидка вне [0, 100).
     */
    Supplier(std::string name, int deliveryDays, double discountPercent);

    /// @brief Название поставщика.
    const std::string& name() const { return name_; }
    /// @brief Срок доставки, дней.
    int deliveryDays() const { return deliveryDays_; }
    /// @brief Средний рейтинг (0, если оценок ещё нет).
    double rating() const { return rating_; }

    /**
     * @brief Стоимость партии с учётом скидки.
     * @param kg Количество, кг (положительное).
     * @param pricePerKg Цена за кг (неотрицательная).
     * @return Итоговая стоимость.
     * @throws InvalidParameterException если количество не положительно или цена отрицательна.
     */
    double quote(double kg, double pricePerKg) const;
    /**
     * @brief Дата доставки заказа.
     * @param orderDate Дата заказа.
     * @return Дата заказа плюс срок доставки.
     */
    Date deliveryDate(const Date& orderDate) const;
    /**
     * @brief Поставляет партию корма; срок годности отсчитывается от даты доставки.
     * @param foodName Название корма.
     * @param kg Количество, кг.
     * @param pricePerKg Цена за кг без скидки.
     * @param caloriesPerKg Калорийность, ккал/кг.
     * @param orderDate Дата заказа.
     * @param shelfLifeDays Срок годности, дней (положительный).
     * @return Партия корма с ценой за кг со скидкой.
     * @throws InvalidParameterException если срок годности не положителен, а также при
     *         недопустимых количестве и цене.
     */
    FoodItem deliver(const std::string& foodName, double kg, double pricePerKg,
                     double caloriesPerKg, const Date& orderDate, int shelfLifeDays) const;
    /**
     * @brief Оценивает поставщика по шкале 1..5; рейтинг — среднее оценок.
     * @param score Оценка от 1 до 5.
     * @throws InvalidParameterException если оценка вне диапазона [1, 5].
     */
    void rate(int score);

private:
    std::string name_;        ///< Название поставщика.
    int deliveryDays_;        ///< Срок доставки, дней.
    double discountPercent_;  ///< Скидка, %.
    double rating_;           ///< Средний рейтинг.
    int ratingsCount_;        ///< Число полученных оценок.
};

}  // namespace ocean
