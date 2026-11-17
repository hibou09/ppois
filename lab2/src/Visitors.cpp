#include "Visitors.h"

#include <algorithm>
#include <utility>

#include "Animals.h"
#include "Exceptions.h"
#include "Tanks.h"

namespace ocean {

// ---------------------------------------------------------------- Visitor

Visitor::Visitor(int id, std::string name, int age, std::string email, double balance)
    : id_(id), name_(std::move(name)), age_(age), email_(std::move(email)), balance_(balance) {
    if (id <= 0 || name_.empty()) {
        throw InvalidParameterException("visitor needs positive id and name");
    }
    if (age < 0 || age > 120 || balance < 0.0) {
        throw InvalidParameterException("visitor age/balance invalid");
    }
}

bool Visitor::isChild() const {
    return age_ < 12;
}

bool Visitor::isSenior() const {
    return age_ >= 65;
}

void Visitor::pay(double amount) {
    if (amount < 0.0) {
        throw InvalidParameterException("payment must be non-negative");
    }
    if (amount > balance_) {
        throw InsufficientFundsException(amount, balance_);
    }
    balance_ -= amount;
}

void Visitor::topUp(double amount) {
    if (amount <= 0.0) {
        throw InvalidParameterException("top-up must be positive");
    }
    balance_ += amount;
}

bool Visitor::hasValidEmail() const {
    auto at = email_.find('@');
    if (at == std::string::npos || at == 0) {
        return false;
    }
    auto dot = email_.find('.', at);
    return dot != std::string::npos && dot > at + 1 && dot + 1 < email_.size();
}

// ---------------------------------------------------------------- Ticket

Ticket::Ticket(int id, TicketType type, double price, const Date& visitDate, int visitorId)
    : id_(id), type_(type), price_(price), visitDate_(visitDate), visitorId_(visitorId),
      used_(false) {
    if (price < 0.0) {
        throw InvalidParameterException("ticket price must be non-negative");
    }
}

bool Ticket::isValidOn(const Date& date) const {
    return !used_ && date == visitDate_;
}

void Ticket::use(const Date& date) {
    if (used_) {
        throw InvalidTicketException("ticket #" + std::to_string(id_) + " already used");
    }
    if (date != visitDate_) {
        throw InvalidTicketException("ticket #" + std::to_string(id_) + " is valid only on " +
                                     visitDate_.toString());
    }
    used_ = true;
}

bool Ticket::isRefundable(const Date& today) const {
    return !used_ && today.isBefore(visitDate_);
}

void Ticket::invalidate() {
    used_ = true;
}

int Ticket::personCount() const {
    return type_ == TicketType::Family ? 4 : 1;
}

// ---------------------------------------------------------------- PromoCode

PromoCode::PromoCode(std::string code, double percent, int usesLeft, const Date& expiry)
    : code_(std::move(code)), percent_(percent), usesLeft_(usesLeft), expiry_(expiry) {
    if (code_.empty() || percent <= 0.0 || percent > 90.0 || usesLeft < 0) {
        throw InvalidParameterException("invalid promo code values");
    }
}

bool PromoCode::isValid(const Date& today) const {
    return usesLeft_ > 0 && !expiry_.isBefore(today);
}

double PromoCode::discounted(double price) const {
    return price * (1.0 - percent_ / 100.0);
}

double PromoCode::apply(double price, const Date& today) {
    if (!isValid(today)) {
        throw InvalidPromoCodeException(code_);
    }
    --usesLeft_;
    return discounted(price);
}

// ---------------------------------------------------------------- Membership

Membership::Membership(int visitorId, MembershipLevel level, const Date& expiry)
    : visitorId_(visitorId), level_(level), expiry_(expiry), visits_(0) {}

bool Membership::isActive(const Date& today) const {
    return !expiry_.isBefore(today);
}

double Membership::discountPercent() const {
    switch (level_) {
        case MembershipLevel::Basic: return 10.0;
        case MembershipLevel::Silver: return 20.0;
        case MembershipLevel::Gold: return 30.0;
    }
    return 0.0;
}

double Membership::applyDiscount(double price, const Date& today) const {
    return isActive(today) ? price * (1.0 - discountPercent() / 100.0) : price;
}

void Membership::renew(int days) {
    if (days <= 0) {
        throw InvalidParameterException("renewal days must be positive");
    }
    expiry_ = expiry_.addDays(days);
}

bool Membership::upgrade() {
    if (level_ == MembershipLevel::Basic) {
        level_ = MembershipLevel::Silver;
        return true;
    }
    if (level_ == MembershipLevel::Silver) {
        level_ = MembershipLevel::Gold;
        return true;
    }
    return false;
}

void Membership::registerVisit(const Date& today) {
    if (!isActive(today)) {
        throw InvalidTicketException("membership expired on " + expiry_.toString());
    }
    ++visits_;
}

// ---------------------------------------------------------------- TicketOffice

TicketOffice::TicketOffice(double basePrice, int dailyLimit)
    : basePrice_(basePrice), dailyLimit_(dailyLimit), revenue_(0.0), nextTicketId_(1),
      refundsCount_(0) {
    if (basePrice <= 0.0 || dailyLimit <= 0) {
        throw InvalidParameterException("ticket price and daily limit must be positive");
    }
}

double TicketOffice::priceFor(const Visitor& visitor, TicketType type) const {
    switch (type) {
        case TicketType::Adult:
            return basePrice_;
        case TicketType::Child:
            if (!visitor.isChild()) {
                throw InvalidTicketException(visitor.name() + " is not eligible for child ticket");
            }
            return basePrice_ * 0.5;
        case TicketType::Senior:
            if (!visitor.isSenior()) {
                throw InvalidTicketException(visitor.name() +
                                             " is not eligible for senior ticket");
            }
            return basePrice_ * 0.6;
        case TicketType::Family:
            return basePrice_ * 3.0;
    }
    return basePrice_;
}

Ticket TicketOffice::sell(Visitor& visitor, TicketType type, const Date& date,
                          const Membership* membership, PromoCode* promo) {
    int persons = type == TicketType::Family ? 4 : 1;
    if (soldOn(date) + persons > dailyLimit_) {
        throw TicketSoldOutException("no tickets left for " + date.toString());
    }
    double price = priceFor(visitor, type);
    if (membership != nullptr && membership->visitorId() == visitor.id()) {
        price = membership->applyDiscount(price, date);
    }
    if (promo != nullptr) {
        if (!promo->isValid(date)) {
            throw InvalidPromoCodeException(promo->code());
        }
        price = promo->discounted(price);
    }
    visitor.pay(price);  // может бросить InsufficientFundsException
    if (promo != nullptr) {
        promo->apply(price, date);
    }
    soldPerDay_[date.toDayNumber()] += persons;
    revenue_ += price;
    return Ticket(nextTicketId_++, type, price, date, visitor.id());
}

void TicketOffice::refund(Ticket& ticket, Visitor& visitor, const Date& today) {
    if (ticket.visitorId() != visitor.id()) {
        throw InvalidTicketException("ticket belongs to another visitor");
    }
    if (!ticket.isRefundable(today)) {
        throw InvalidTicketException("ticket #" + std::to_string(ticket.id()) +
                                     " cannot be refunded");
    }
    if (ticket.price() > 0.0) {
        visitor.topUp(ticket.price());
    }
    revenue_ -= ticket.price();
    soldPerDay_[ticket.visitDate().toDayNumber()] -= ticket.personCount();
    ticket.invalidate();
    ++refundsCount_;
}

int TicketOffice::soldOn(const Date& date) const {
    auto it = soldPerDay_.find(date.toDayNumber());
    return it == soldPerDay_.end() ? 0 : it->second;
}

int TicketOffice::remainingFor(const Date& date) const {
    return dailyLimit_ - soldOn(date);
}

// ---------------------------------------------------------------- Product

Product::Product(std::string name, std::string category, double price, int stock)
    : name_(std::move(name)), category_(std::move(category)), price_(price), stock_(stock),
      soldCount_(0) {
    if (name_.empty() || price < 0.0 || stock < 0) {
        throw InvalidParameterException("invalid product values");
    }
}

double Product::sell(int qty) {
    if (qty <= 0) {
        throw InvalidParameterException("quantity must be positive");
    }
    if (qty > stock_) {
        throw OutOfStockException(name_ + ": requested " + std::to_string(qty) + ", in stock " +
                                  std::to_string(stock_));
    }
    stock_ -= qty;
    soldCount_ += qty;
    return price_ * qty;
}

void Product::restock(int qty) {
    if (qty <= 0) {
        throw InvalidParameterException("restock quantity must be positive");
    }
    stock_ += qty;
}

void Product::applyMarkdown(double percent) {
    if (percent <= 0.0 || percent >= 100.0) {
        throw InvalidParameterException("markdown must be in (0, 100)");
    }
    price_ *= (1.0 - percent / 100.0);
}

// ---------------------------------------------------------------- GiftShop

GiftShop::GiftShop(std::string name) : name_(std::move(name)), revenue_(0.0), transactions_(0) {}

void GiftShop::addProduct(const Product& product) {
    for (const auto& p : products_) {
        if (p.name() == product.name()) {
            throw InvalidParameterException("product " + product.name() + " already exists");
        }
    }
    products_.push_back(product);
}

Product& GiftShop::find(const std::string& name) {
    for (auto& p : products_) {
        if (p.name() == name) {
            return p;
        }
    }
    throw OutOfStockException("product " + name + " is not sold here");
}

double GiftShop::sell(const std::string& productName, int qty, Visitor& visitor) {
    Product& product = find(productName);
    if (qty <= 0) {
        throw InvalidParameterException("quantity must be positive");
    }
    if (qty > product.stock()) {
        throw OutOfStockException(productName);
    }
    double cost = product.price() * qty;
    visitor.pay(cost);
    product.sell(qty);
    revenue_ += cost;
    ++transactions_;
    return cost;
}

std::string GiftShop::bestSeller() const {
    const Product* best = nullptr;
    for (const auto& p : products_) {
        if (p.soldCount() > 0 && (best == nullptr || p.soldCount() > best->soldCount())) {
            best = &p;
        }
    }
    return best == nullptr ? std::string() : best->name();
}

double GiftShop::inventoryValue() const {
    double total = 0.0;
    for (const auto& p : products_) {
        total += p.price() * p.stock();
    }
    return total;
}

std::vector<std::string> GiftShop::lowStock(int threshold) const {
    std::vector<std::string> result;
    for (const auto& p : products_) {
        if (p.stock() <= threshold) {
            result.push_back(p.name());
        }
    }
    return result;
}

// ---------------------------------------------------------------- Tour

Tour::Tour(int id, const TimeSlot& slot, int maxGroupSize)
    : id_(id), slot_(slot), maxGroupSize_(maxGroupSize), guide_(nullptr), completed_(false) {
    if (maxGroupSize <= 0) {
        throw InvalidParameterException("group size must be positive");
    }
}

void Tour::assignGuide(Guide* guide) {
    if (guide == nullptr) {
        throw InvalidParameterException("guide must not be null");
    }
    guide_ = guide;
}

void Tour::addVisitor(Visitor& visitor) {
    if (completed_) {
        throw InvalidParameterException("tour already completed");
    }
    if (groupSize() >= maxGroupSize_) {
        throw EventFullException("tour #" + std::to_string(id_) + " is full");
    }
    for (const Visitor* v : visitors_) {
        if (v->id() == visitor.id()) {
            throw InvalidParameterException(visitor.name() + " already in tour");
        }
    }
    visitors_.push_back(&visitor);
}

bool Tour::removeVisitor(int visitorId) {
    auto it = std::find_if(visitors_.begin(), visitors_.end(),
                           [visitorId](const Visitor* v) { return v->id() == visitorId; });
    if (it == visitors_.end()) {
        return false;
    }
    visitors_.erase(it);
    return true;
}

void Tour::addStop(Tank* tank) {
    if (tank == nullptr) {
        throw InvalidParameterException("tank must not be null");
    }
    if (!tank->isOpen()) {
        throw InvalidParameterException("tank " + tank->name() + " is closed for visitors");
    }
    route_.push_back(tank);
}

int Tour::groupSize() const {
    return static_cast<int>(visitors_.size());
}

int Tour::estimatedDurationMinutes() const {
    return static_cast<int>(route_.size()) * 15;
}

bool Tour::fitsInSlot() const {
    return estimatedDurationMinutes() <= slot_.durationMinutes();
}

void Tour::complete() {
    if (guide_ == nullptr) {
        throw InvalidParameterException("tour has no guide");
    }
    if (route_.empty()) {
        throw InvalidParameterException("tour has no stops");
    }
    completed_ = true;
}

// ---------------------------------------------------------------- Show

Show::Show(std::string title, const TimeSlot& slot, int capacity)
    : title_(std::move(title)), slot_(slot), capacity_(capacity), seatsTaken_(0),
      cancelled_(false) {
    if (title_.empty() || capacity <= 0) {
        throw InvalidParameterException("show needs title and positive capacity");
    }
}

void Show::addPerformer(Seal* seal) {
    if (seal == nullptr) {
        throw InvalidParameterException("performer must not be null");
    }
    performers_.push_back(seal);
}

void Show::bookSeats(int count) {
    if (cancelled_) {
        throw InvalidParameterException("show " + title_ + " is cancelled");
    }
    if (count <= 0) {
        throw InvalidParameterException("seat count must be positive");
    }
    if (seatsTaken_ + count > capacity_) {
        throw EventFullException("show " + title_ + " has only " +
                                 std::to_string(capacity_ - seatsTaken_) + " seats left");
    }
    seatsTaken_ += count;
}

void Show::releaseSeats(int count) {
    if (count <= 0 || count > seatsTaken_) {
        throw InvalidParameterException("cannot release that many seats");
    }
    seatsTaken_ -= count;
}

void Show::cancel() {
    cancelled_ = true;
    seatsTaken_ = 0;
}

bool Show::isFull() const {
    return seatsTaken_ >= capacity_;
}

double Show::occupancyRate() const {
    return static_cast<double>(seatsTaken_) / capacity_;
}

}  // namespace ocean
