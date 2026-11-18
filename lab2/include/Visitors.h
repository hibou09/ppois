/**
 * @file Visitors.h
 * @brief Посетители, билеты, абонементы, магазин, экскурсии и шоу.
 */
#pragma once

#include <map>
#include <string>
#include <vector>

#include "Common.h"

namespace ocean {

class Guide;
class Tank;
class Seal;

/// @brief Посетитель океанариума.
class Visitor {
public:
    /**
     * @brief Создаёт посетителя.
     * @param id Положительный идентификатор.
     * @param name Непустое имя.
     * @param age Возраст, [0; 120].
     * @param email Адрес электронной почты.
     * @param balance Начальный баланс (>= 0).
     * @throws InvalidParameterException при неположительном id, пустом имени, недопустимом
     * возрасте или отрицательном балансе.
     */
    Visitor(int id, std::string name, int age, std::string email, double balance);

    /// @brief Идентификатор посетителя.
    int id() const { return id_; }
    /// @brief Имя посетителя.
    const std::string& name() const { return name_; }
    /// @brief Возраст.
    int age() const { return age_; }
    /// @brief Адрес электронной почты.
    const std::string& email() const { return email_; }
    /// @brief Текущий баланс.
    double balance() const { return balance_; }

    /// @brief true, если посетителю меньше 12 лет.
    bool isChild() const;
    /// @brief true, если посетителю 65 лет и больше.
    bool isSenior() const;
    /**
     * @brief Списывает деньги с баланса.
     * @param amount Сумма (>= 0).
     * @throws InvalidParameterException если сумма отрицательна.
     * @throws InsufficientFundsException если на балансе недостаточно средств.
     */
    void pay(double amount);
    /**
     * @brief Пополняет баланс.
     * @param amount Сумма (> 0).
     * @throws InvalidParameterException если сумма не положительна.
     */
    void topUp(double amount);
    /// @brief Простая проверка формата адреса электронной почты (имя@домен.зона).
    bool hasValidEmail() const;

private:
    int id_;            ///< Идентификатор.
    std::string name_;  ///< Имя.
    int age_;           ///< Возраст.
    std::string email_; ///< Электронная почта.
    double balance_;    ///< Баланс.
};

/// @brief Тип входного билета.
enum class TicketType {
    Adult,   ///< Взрослый.
    Child,   ///< Детский (для посетителей младше 12 лет).
    Senior,  ///< Пенсионный (для посетителей от 65 лет).
    Family   ///< Семейный (на 4 человека).
};

/// @brief Входной билет на конкретную дату.
class Ticket {
public:
    /**
     * @brief Создаёт билет.
     * @param id Идентификатор билета.
     * @param type Тип билета.
     * @param price Цена (>= 0).
     * @param visitDate Дата посещения.
     * @param visitorId Идентификатор владельца.
     * @throws InvalidParameterException если цена отрицательна.
     */
    Ticket(int id, TicketType type, double price, const Date& visitDate, int visitorId);

    /// @brief Идентификатор билета.
    int id() const { return id_; }
    /// @brief Тип билета.
    TicketType type() const { return type_; }
    /// @brief Цена билета.
    double price() const { return price_; }
    /// @brief Дата посещения.
    const Date& visitDate() const { return visitDate_; }
    /// @brief Идентификатор владельца.
    int visitorId() const { return visitorId_; }
    /// @brief Использован ли (или аннулирован) билет.
    bool isUsed() const { return used_; }

    /// @brief Действует ли билет в указанную дату (не использован и дата совпадает).
    /// @param date Проверяемая дата.
    bool isValidOn(const Date& date) const;
    /**
     * @brief Пройти по билету; бросает InvalidTicketException.
     * @param date Дата прохода.
     * @throws InvalidTicketException если билет уже использован или дата не совпадает с датой билета.
     */
    void use(const Date& date);
    /// @brief Можно ли вернуть билет: он не использован и дата посещения ещё не наступила.
    /// @param today Текущая дата.
    bool isRefundable(const Date& today) const;
    /// @brief Аннулирует билет (помечает использованным).
    void invalidate();
    /// @brief Сколько человек проходит по билету (4 для семейного, иначе 1).
    int personCount() const;

private:
    int id_;             ///< Идентификатор.
    TicketType type_;    ///< Тип.
    double price_;       ///< Цена.
    Date visitDate_;     ///< Дата посещения.
    int visitorId_;      ///< Владелец.
    bool used_;          ///< Использован или аннулирован.
};

/// @brief Промокод на скидку.
class PromoCode {
public:
    /**
     * @brief Создаёт промокод.
     * @param code Непустой код.
     * @param percent Размер скидки в процентах, (0; 90].
     * @param usesLeft Сколько использований осталось (>= 0).
     * @param expiry Последний день действия.
     * @throws InvalidParameterException если код пуст, скидка вне (0; 90] или usesLeft отрицательно.
     */
    PromoCode(std::string code, double percent, int usesLeft, const Date& expiry);

    /// @brief Текст промокода.
    const std::string& code() const { return code_; }
    /// @brief Размер скидки, %.
    double percent() const { return percent_; }
    /// @brief Сколько использований осталось.
    int usesLeft() const { return usesLeft_; }

    /// @brief Действует ли промокод: остались использования и срок не истёк.
    /// @param today Текущая дата.
    bool isValid(const Date& today) const;
    /// @brief Цена после скидки (без расхода использования).
    /// @param price Исходная цена.
    double discounted(double price) const;
    /**
     * @brief Применить промокод (расходует одно использование).
     * @param price Исходная цена.
     * @param today Текущая дата.
     * @return Цена со скидкой.
     * @throws InvalidPromoCodeException если промокод недействителен.
     */
    double apply(double price, const Date& today);

private:
    std::string code_;  ///< Код.
    double percent_;    ///< Скидка, %.
    int usesLeft_;      ///< Оставшиеся использования.
    Date expiry_;       ///< Последний день действия.
};

/// @brief Уровень абонемента.
enum class MembershipLevel {
    Basic,   ///< Базовый (скидка 10%).
    Silver,  ///< Серебряный (скидка 20%).
    Gold     ///< Золотой (скидка 30%).
};

/// @brief Абонемент постоянного посетителя.
class Membership {
public:
    /**
     * @brief Создаёт абонемент.
     * @param visitorId Идентификатор владельца.
     * @param level Уровень абонемента.
     * @param expiry Последний день действия.
     */
    Membership(int visitorId, MembershipLevel level, const Date& expiry);

    /// @brief Идентификатор владельца.
    int visitorId() const { return visitorId_; }
    /// @brief Уровень абонемента.
    MembershipLevel level() const { return level_; }
    /// @brief Последний день действия.
    const Date& expiry() const { return expiry_; }
    /// @brief Число зарегистрированных визитов.
    int visits() const { return visits_; }

    /// @brief Действует ли абонемент на указанную дату.
    /// @param today Текущая дата.
    bool isActive(const Date& today) const;
    /// @brief Скидка уровня абонемента в процентах (10, 20 или 30).
    double discountPercent() const;
    /// @brief Цена со скидкой, если абонемент действует, иначе исходная цена.
    /// @param price Исходная цена.
    /// @param today Текущая дата.
    double applyDiscount(double price, const Date& today) const;
    /**
     * @brief Продлевает абонемент.
     * @param days Число дней (> 0).
     * @throws InvalidParameterException если days не положительно.
     */
    void renew(int days);
    /// @brief Повышает уровень на ступень (Basic → Silver → Gold).
    /// @return true, если уровень повышен; false, если уже максимальный.
    bool upgrade();
    /**
     * @brief Регистрирует визит по абонементу.
     * @param today Текущая дата.
     * @throws InvalidTicketException если абонемент истёк.
     */
    void registerVisit(const Date& today);

private:
    int visitorId_;         ///< Владелец.
    MembershipLevel level_; ///< Уровень.
    Date expiry_;           ///< Последний день действия.
    int visits_;            ///< Число визитов.
};

/// @brief Билетная касса.
class TicketOffice {
public:
    /**
     * @brief Создаёт кассу.
     * @param basePrice Базовая цена взрослого билета (> 0).
     * @param dailyLimit Лимит посетителей в день (> 0).
     * @throws InvalidParameterException если цена или лимит не положительны.
     */
    TicketOffice(double basePrice, int dailyLimit);

    /// @brief Выручка кассы с учётом возвратов.
    double revenue() const { return revenue_; }
    /// @brief Число оформленных возвратов.
    int refundsCount() const { return refundsCount_; }
    /// @brief Базовая цена билета.
    double basePrice() const { return basePrice_; }

    /**
     * @brief Цена билета указанного типа для посетителя (без скидок).
     * @param visitor Покупатель.
     * @param type Тип билета.
     * @return Цена: базовая для взрослого, 50% для детского, 60% для пенсионного, 300% для семейного.
     * @throws InvalidTicketException если посетитель не подходит под детский или пенсионный билет.
     */
    double priceFor(const Visitor& visitor, TicketType type) const;
    /**
     * @brief Продать билет; учитывает абонемент и промокод.
     * @param visitor Покупатель (с его баланса списывается стоимость).
     * @param type Тип билета.
     * @param date Дата посещения.
     * @param membership Абонемент покупателя или nullptr; скидка применяется, если он принадлежит покупателю и действует.
     * @param promo Промокод или nullptr.
     * @return Оформленный билет.
     * @throws TicketSoldOutException если на дату не хватает мест.
     * @throws InvalidTicketException если посетитель не подходит под тип билета.
     * @throws InvalidPromoCodeException если промокод недействителен.
     * @throws InsufficientFundsException если на балансе недостаточно средств.
     */
    Ticket sell(Visitor& visitor, TicketType type, const Date& date,
                const Membership* membership = nullptr, PromoCode* promo = nullptr);
    /**
     * @brief Возвращает билет: деньги возвращаются на баланс, билет аннулируется.
     * @param ticket Возвращаемый билет.
     * @param visitor Владелец билета.
     * @param today Текущая дата.
     * @throws InvalidTicketException если билет принадлежит другому посетителю или возврат невозможен.
     */
    void refund(Ticket& ticket, Visitor& visitor, const Date& today);
    /// @brief Сколько человек уже купило билеты на дату.
    /// @param date Дата посещения.
    int soldOn(const Date& date) const;
    /// @brief Сколько мест осталось на дату.
    /// @param date Дата посещения.
    int remainingFor(const Date& date) const;

private:
    double basePrice_;                ///< Базовая цена.
    int dailyLimit_;                  ///< Лимит посетителей в день.
    double revenue_;                  ///< Выручка.
    int nextTicketId_;                ///< Следующий идентификатор билета.
    int refundsCount_;                ///< Число возвратов.
    std::map<long, int> soldPerDay_;  ///< Проданные места по номеру дня.
};

/// @brief Товар сувенирного магазина.
class Product {
public:
    /**
     * @brief Создаёт товар.
     * @param name Непустое название.
     * @param category Категория.
     * @param price Цена (>= 0).
     * @param stock Остаток на складе (>= 0).
     * @throws InvalidParameterException если название пустое, цена или остаток отрицательны.
     */
    Product(std::string name, std::string category, double price, int stock);

    /// @brief Название товара.
    const std::string& name() const { return name_; }
    /// @brief Категория товара.
    const std::string& category() const { return category_; }
    /// @brief Цена за штуку.
    double price() const { return price_; }
    /// @brief Остаток на складе.
    int stock() const { return stock_; }
    /// @brief Сколько штук продано.
    int soldCount() const { return soldCount_; }

    /**
     * @brief Продать qty штук, вернуть сумму.
     * @param qty Количество (> 0).
     * @return Стоимость проданного.
     * @throws InvalidParameterException если количество не положительно.
     * @throws OutOfStockException если на складе меньше qty.
     */
    double sell(int qty);
    /**
     * @brief Пополняет склад.
     * @param qty Количество (> 0).
     * @throws InvalidParameterException если количество не положительно.
     */
    void restock(int qty);
    /**
     * @brief Снижает цену на процент.
     * @param percent Размер уценки, (0; 100).
     * @throws InvalidParameterException если процент вне (0; 100).
     */
    void applyMarkdown(double percent);

private:
    std::string name_;      ///< Название.
    std::string category_;  ///< Категория.
    double price_;          ///< Цена.
    int stock_;             ///< Остаток.
    int soldCount_;         ///< Продано штук.
};

/// @brief Сувенирный магазин.
class GiftShop {
public:
    /// @brief Создаёт магазин.
    /// @param name Название магазина.
    explicit GiftShop(std::string name);

    /// @brief Название магазина.
    const std::string& name() const { return name_; }
    /// @brief Выручка магазина.
    double revenue() const { return revenue_; }
    /// @brief Число совершённых продаж.
    int transactions() const { return transactions_; }

    /**
     * @brief Добавляет товар в ассортимент.
     * @param product Добавляемый товар.
     * @throws InvalidParameterException если товар с таким названием уже есть.
     */
    void addProduct(const Product& product);
    /**
     * @brief Находит товар по названию.
     * @param name Название товара.
     * @return Ссылка на товар.
     * @throws OutOfStockException если такого товара в магазине нет.
     */
    Product& find(const std::string& name);
    /**
     * @brief Продаёт товар посетителю.
     * @param productName Название товара.
     * @param qty Количество (> 0).
     * @param visitor Покупатель (с его баланса списывается стоимость).
     * @return Стоимость покупки.
     * @throws OutOfStockException если товара нет в магазине или его не хватает.
     * @throws InvalidParameterException если количество не положительно.
     * @throws InsufficientFundsException если на балансе недостаточно средств.
     */
    double sell(const std::string& productName, int qty, Visitor& visitor);
    /// @brief Название самого продаваемого товара (пустая строка, если продаж не было).
    std::string bestSeller() const;
    /// @brief Стоимость остатков на складе.
    double inventoryValue() const;
    /// @brief Названия товаров, остаток которых не больше порога.
    /// @param threshold Порог остатка.
    std::vector<std::string> lowStock(int threshold) const;

private:
    std::string name_;              ///< Название.
    std::vector<Product> products_; ///< Ассортимент.
    double revenue_;                ///< Выручка.
    int transactions_;              ///< Число продаж.
};

/// @brief Экскурсия по аквариумам.
class Tour {
public:
    /**
     * @brief Создаёт экскурсию.
     * @param id Идентификатор.
     * @param slot Временной интервал.
     * @param maxGroupSize Максимальный размер группы (> 0).
     * @throws InvalidParameterException если размер группы не положителен.
     */
    Tour(int id, const TimeSlot& slot, int maxGroupSize);

    /// @brief Идентификатор экскурсии.
    int id() const { return id_; }
    /// @brief Временной интервал.
    const TimeSlot& slot() const { return slot_; }
    /// @brief Назначенный гид (nullptr, если не назначен).
    Guide* guide() const { return guide_; }
    /// @brief Проведена ли экскурсия.
    bool isCompleted() const { return completed_; }
    /// @brief Маршрут — список аквариумов.
    const std::vector<Tank*>& route() const { return route_; }

    /**
     * @brief Назначает гида (без передачи владения).
     * @param guide Гид.
     * @throws InvalidParameterException если указатель пуст.
     */
    void assignGuide(Guide* guide);
    /**
     * @brief Записывает посетителя в группу.
     * @param visitor Посетитель.
     * @throws InvalidParameterException если экскурсия уже проведена или посетитель уже в группе.
     * @throws EventFullException если группа заполнена.
     */
    void addVisitor(Visitor& visitor);
    /// @brief Исключает посетителя из группы.
    /// @param visitorId Идентификатор посетителя.
    /// @return true, если посетитель был в группе.
    bool removeVisitor(int visitorId);
    /**
     * @brief Добавляет остановку в маршрут.
     * @param tank Аквариум.
     * @throws InvalidParameterException если указатель пуст или аквариум закрыт для посетителей.
     */
    void addStop(Tank* tank);
    /// @brief Текущий размер группы.
    int groupSize() const;
    /// @brief Оценка длительности: 15 минут на остановку.
    int estimatedDurationMinutes() const;
    /// @brief true, если маршрут укладывается во временной интервал.
    bool fitsInSlot() const;
    /**
     * @brief Завершает экскурсию.
     * @throws InvalidParameterException если не назначен гид или в маршруте нет остановок.
     */
    void complete();

private:
    int id_;                         ///< Идентификатор.
    TimeSlot slot_;                  ///< Временной интервал.
    int maxGroupSize_;               ///< Максимальный размер группы.
    Guide* guide_;                   ///< Гид (не владеет).
    std::vector<Visitor*> visitors_; ///< Группа (не владеет).
    std::vector<Tank*> route_;       ///< Маршрут (не владеет).
    bool completed_;                 ///< Проведена ли экскурсия.
};

/// @brief Шоу морских животных.
class Show {
public:
    /**
     * @brief Создаёт шоу.
     * @param title Непустое название.
     * @param slot Временной интервал.
     * @param capacity Число мест (> 0).
     * @throws InvalidParameterException если название пустое или число мест не положительно.
     */
    Show(std::string title, const TimeSlot& slot, int capacity);

    /// @brief Название шоу.
    const std::string& title() const { return title_; }
    /// @brief Временной интервал.
    const TimeSlot& slot() const { return slot_; }
    /// @brief Сколько мест занято.
    int seatsTaken() const { return seatsTaken_; }
    /// @brief Отменено ли шоу.
    bool isCancelled() const { return cancelled_; }
    /// @brief Артисты шоу (тюлени).
    const std::vector<Seal*>& performers() const { return performers_; }

    /**
     * @brief Добавляет артиста (без передачи владения).
     * @param seal Тюлень.
     * @throws InvalidParameterException если указатель пуст.
     */
    void addPerformer(Seal* seal);
    /**
     * @brief Бронирует места.
     * @param count Число мест (> 0).
     * @throws InvalidParameterException если шоу отменено или число мест не положительно.
     * @throws EventFullException если свободных мест недостаточно.
     */
    void bookSeats(int count);
    /**
     * @brief Освобождает места.
     * @param count Число мест (> 0 и не больше занятых).
     * @throws InvalidParameterException если число мест некорректно.
     */
    void releaseSeats(int count);
    /// @brief Отменяет шоу и освобождает все места.
    void cancel();
    /// @brief true, если все места заняты.
    bool isFull() const;
    /// @brief Заполненность зала, от 0 до 1.
    double occupancyRate() const;

private:
    std::string title_;              ///< Название.
    TimeSlot slot_;                  ///< Временной интервал.
    int capacity_;                   ///< Число мест.
    int seatsTaken_;                 ///< Занятые места.
    std::vector<Seal*> performers_;  ///< Артисты (не владеют).
    bool cancelled_;                 ///< Отменено ли шоу.
};

}  // namespace ocean
