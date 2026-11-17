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

/// Посетитель океанариума.
class Visitor {
public:
    Visitor(int id, std::string name, int age, std::string email, double balance);

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    int age() const { return age_; }
    const std::string& email() const { return email_; }
    double balance() const { return balance_; }

    bool isChild() const;
    bool isSenior() const;
    void pay(double amount);
    void topUp(double amount);
    bool hasValidEmail() const;

private:
    int id_;
    std::string name_;
    int age_;
    std::string email_;
    double balance_;
};

enum class TicketType { Adult, Child, Senior, Family };

/// Входной билет на конкретную дату.
class Ticket {
public:
    Ticket(int id, TicketType type, double price, const Date& visitDate, int visitorId);

    int id() const { return id_; }
    TicketType type() const { return type_; }
    double price() const { return price_; }
    const Date& visitDate() const { return visitDate_; }
    int visitorId() const { return visitorId_; }
    bool isUsed() const { return used_; }

    bool isValidOn(const Date& date) const;
    /// Пройти по билету; бросает InvalidTicketException.
    void use(const Date& date);
    bool isRefundable(const Date& today) const;
    void invalidate();
    /// Сколько человек проходит по билету.
    int personCount() const;

private:
    int id_;
    TicketType type_;
    double price_;
    Date visitDate_;
    int visitorId_;
    bool used_;
};

/// Промокод на скидку.
class PromoCode {
public:
    PromoCode(std::string code, double percent, int usesLeft, const Date& expiry);

    const std::string& code() const { return code_; }
    double percent() const { return percent_; }
    int usesLeft() const { return usesLeft_; }

    bool isValid(const Date& today) const;
    double discounted(double price) const;
    /// Применить промокод (расходует одно использование).
    double apply(double price, const Date& today);

private:
    std::string code_;
    double percent_;
    int usesLeft_;
    Date expiry_;
};

enum class MembershipLevel { Basic, Silver, Gold };

/// Абонемент постоянного посетителя.
class Membership {
public:
    Membership(int visitorId, MembershipLevel level, const Date& expiry);

    int visitorId() const { return visitorId_; }
    MembershipLevel level() const { return level_; }
    const Date& expiry() const { return expiry_; }
    int visits() const { return visits_; }

    bool isActive(const Date& today) const;
    double discountPercent() const;
    double applyDiscount(double price, const Date& today) const;
    void renew(int days);
    bool upgrade();
    void registerVisit(const Date& today);

private:
    int visitorId_;
    MembershipLevel level_;
    Date expiry_;
    int visits_;
};

/// Билетная касса.
class TicketOffice {
public:
    TicketOffice(double basePrice, int dailyLimit);

    double revenue() const { return revenue_; }
    int refundsCount() const { return refundsCount_; }
    double basePrice() const { return basePrice_; }

    double priceFor(const Visitor& visitor, TicketType type) const;
    /// Продать билет; учитывает абонемент и промокод.
    Ticket sell(Visitor& visitor, TicketType type, const Date& date,
                const Membership* membership = nullptr, PromoCode* promo = nullptr);
    void refund(Ticket& ticket, Visitor& visitor, const Date& today);
    int soldOn(const Date& date) const;
    int remainingFor(const Date& date) const;

private:
    double basePrice_;
    int dailyLimit_;
    double revenue_;
    int nextTicketId_;
    int refundsCount_;
    std::map<long, int> soldPerDay_;
};

/// Товар сувенирного магазина.
class Product {
public:
    Product(std::string name, std::string category, double price, int stock);

    const std::string& name() const { return name_; }
    const std::string& category() const { return category_; }
    double price() const { return price_; }
    int stock() const { return stock_; }
    int soldCount() const { return soldCount_; }

    /// Продать qty штук, вернуть сумму.
    double sell(int qty);
    void restock(int qty);
    void applyMarkdown(double percent);

private:
    std::string name_;
    std::string category_;
    double price_;
    int stock_;
    int soldCount_;
};

/// Сувенирный магазин.
class GiftShop {
public:
    explicit GiftShop(std::string name);

    const std::string& name() const { return name_; }
    double revenue() const { return revenue_; }
    int transactions() const { return transactions_; }

    void addProduct(const Product& product);
    Product& find(const std::string& name);
    double sell(const std::string& productName, int qty, Visitor& visitor);
    std::string bestSeller() const;
    double inventoryValue() const;
    std::vector<std::string> lowStock(int threshold) const;

private:
    std::string name_;
    std::vector<Product> products_;
    double revenue_;
    int transactions_;
};

/// Экскурсия по аквариумам.
class Tour {
public:
    Tour(int id, const TimeSlot& slot, int maxGroupSize);

    int id() const { return id_; }
    const TimeSlot& slot() const { return slot_; }
    Guide* guide() const { return guide_; }
    bool isCompleted() const { return completed_; }
    const std::vector<Tank*>& route() const { return route_; }

    void assignGuide(Guide* guide);
    void addVisitor(Visitor& visitor);
    bool removeVisitor(int visitorId);
    void addStop(Tank* tank);
    int groupSize() const;
    int estimatedDurationMinutes() const;
    bool fitsInSlot() const;
    void complete();

private:
    int id_;
    TimeSlot slot_;
    int maxGroupSize_;
    Guide* guide_;
    std::vector<Visitor*> visitors_;
    std::vector<Tank*> route_;
    bool completed_;
};

/// Шоу морских животных.
class Show {
public:
    Show(std::string title, const TimeSlot& slot, int capacity);

    const std::string& title() const { return title_; }
    const TimeSlot& slot() const { return slot_; }
    int seatsTaken() const { return seatsTaken_; }
    bool isCancelled() const { return cancelled_; }
    const std::vector<Seal*>& performers() const { return performers_; }

    void addPerformer(Seal* seal);
    void bookSeats(int count);
    void releaseSeats(int count);
    void cancel();
    bool isFull() const;
    double occupancyRate() const;

private:
    std::string title_;
    TimeSlot slot_;
    int capacity_;
    int seatsTaken_;
    std::vector<Seal*> performers_;
    bool cancelled_;
};

}  // namespace ocean
