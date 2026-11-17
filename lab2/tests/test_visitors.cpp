#include <UnitTest++/UnitTest++.h>

#include "Exceptions.h"
#include "Tanks.h"
#include "TestHelpers.h"
#include "Visitors.h"

using namespace ocean;
using namespace testutil;

SUITE(VisitorTests) {
    TEST(AgeCategories) {
        CHECK(Visitor(1, "Kid", 8, "k@a.by", 0).isChild());
        CHECK(Visitor(2, "Old", 70, "o@a.by", 0).isSenior());
        Visitor adult(3, "Adult", 30, "a@a.by", 0);
        CHECK(!adult.isChild());
        CHECK(!adult.isSenior());
    }

    TEST(PaymentAndTopUp) {
        Visitor v(1, "Anna", 30, "anna@mail.com", 50.0);
        v.pay(20.0);
        CHECK_CLOSE(30.0, v.balance(), 1e-9);
        CHECK_THROW(v.pay(100.0), InsufficientFundsException);
        v.topUp(70.0);
        CHECK_CLOSE(100.0, v.balance(), 1e-9);
        CHECK_THROW(v.topUp(0.0), InvalidParameterException);
    }

    TEST(EmailValidation) {
        CHECK(Visitor(1, "A", 30, "anna@mail.com", 0).hasValidEmail());
        CHECK(!Visitor(1, "A", 30, "annamail.com", 0).hasValidEmail());
        CHECK(!Visitor(1, "A", 30, "anna@com", 0).hasValidEmail());
        CHECK(!Visitor(1, "A", 30, "@mail.com", 0).hasValidEmail());
    }

    TEST(InvalidVisitorThrows) {
        CHECK_THROW(Visitor(1, "A", -1, "", 0), InvalidParameterException);
        CHECK_THROW(Visitor(1, "A", 30, "", -5), InvalidParameterException);
    }
}

SUITE(TicketTests) {
    TEST(UseTicket) {
        Ticket t(1, TicketType::Adult, 25.0, today(), 1);
        CHECK(t.isValidOn(today()));
        CHECK(!t.isValidOn(today().addDays(1)));
        CHECK_THROW(t.use(today().addDays(1)), InvalidTicketException);
        t.use(today());
        CHECK(t.isUsed());
        CHECK_THROW(t.use(today()), InvalidTicketException);
    }

    TEST(RefundabilityAndPersons) {
        Ticket t(1, TicketType::Family, 75.0, today().addDays(5), 1);
        CHECK(t.isRefundable(today()));
        CHECK(!t.isRefundable(today().addDays(5)));
        CHECK_EQUAL(4, t.personCount());
        t.invalidate();
        CHECK(!t.isRefundable(today()));
    }

    TEST(PromoCode) {
        PromoCode p("SEA10", 10.0, 1, today().addDays(10));
        CHECK(p.isValid(today()));
        CHECK_CLOSE(90.0, p.apply(100.0, today()), 1e-9);
        CHECK(!p.isValid(today()));
        CHECK_THROW(p.apply(100.0, today()), InvalidPromoCodeException);
        PromoCode expired("OLD", 10.0, 5, today().addDays(-1));
        CHECK_THROW(expired.apply(100.0, today()), InvalidPromoCodeException);
        CHECK_THROW(ocean::PromoCode("BAD", 95.0, 1, today()), InvalidParameterException);
    }

    TEST(Membership) {
        Membership m(1, MembershipLevel::Basic, today().addDays(30));
        CHECK(m.isActive(today()));
        CHECK_CLOSE(90.0, m.applyDiscount(100.0, today()), 1e-9);
        CHECK(m.upgrade());
        CHECK(m.upgrade());
        CHECK(!m.upgrade());
        CHECK_CLOSE(30.0, m.discountPercent(), 1e-9);
        m.registerVisit(today());
        CHECK_EQUAL(1, m.visits());
        CHECK_CLOSE(100.0, m.applyDiscount(100.0, today().addDays(40)), 1e-9);
        CHECK_THROW(m.registerVisit(today().addDays(40)), InvalidTicketException);
        m.renew(30);
        CHECK(m.isActive(today().addDays(40)));
        CHECK_THROW(m.renew(0), InvalidParameterException);
    }
}

SUITE(TicketOfficeTests) {
    TEST(Prices) {
        TicketOffice office(20.0, 100);
        Visitor adult(1, "A", 30, "a@a.by", 100);
        Visitor kid(2, "K", 8, "k@a.by", 100);
        Visitor senior(3, "S", 70, "s@a.by", 100);
        CHECK_CLOSE(20.0, office.priceFor(adult, TicketType::Adult), 1e-9);
        CHECK_CLOSE(10.0, office.priceFor(kid, TicketType::Child), 1e-9);
        CHECK_CLOSE(12.0, office.priceFor(senior, TicketType::Senior), 1e-9);
        CHECK_CLOSE(60.0, office.priceFor(adult, TicketType::Family), 1e-9);
        CHECK_THROW(office.priceFor(adult, TicketType::Child), InvalidTicketException);
        CHECK_THROW(office.priceFor(adult, TicketType::Senior), InvalidTicketException);
    }

    TEST(SellWithDiscounts) {
        TicketOffice office(20.0, 100);
        Visitor v(1, "A", 30, "a@a.by", 100);
        Membership m(1, MembershipLevel::Silver, today().addDays(30));
        PromoCode p("SEA", 50.0, 1, today().addDays(30));
        Ticket t = office.sell(v, TicketType::Adult, today(), &m, &p);
        CHECK_CLOSE(8.0, t.price(), 1e-9);  // 20 * 0.8 * 0.5
        CHECK_CLOSE(92.0, v.balance(), 1e-9);
        CHECK_EQUAL(0, p.usesLeft());
        CHECK_CLOSE(8.0, office.revenue(), 1e-9);
        CHECK_EQUAL(1, office.soldOn(today()));
    }

    TEST(SoldOutAndFunds) {
        TicketOffice office(20.0, 5);
        Visitor rich(1, "R", 30, "r@a.by", 1000);
        Visitor poor(2, "P", 30, "p@a.by", 5);
        office.sell(rich, TicketType::Family, today());
        CHECK_EQUAL(1, office.remainingFor(today()));
        CHECK_THROW(office.sell(rich, TicketType::Family, today()), TicketSoldOutException);
        CHECK_THROW(office.sell(poor, TicketType::Adult, today()), InsufficientFundsException);
        CHECK_EQUAL(4, office.soldOn(today()));
    }

    TEST(PromoNotConsumedWhenPaymentFails) {
        TicketOffice office(20.0, 5);
        Visitor poor(1, "P", 30, "p@a.by", 1);
        PromoCode p("SEA", 10.0, 1, today().addDays(30));
        CHECK_THROW(office.sell(poor, TicketType::Adult, today(), nullptr, &p),
                    InsufficientFundsException);
        CHECK_EQUAL(1, p.usesLeft());
    }

    TEST(Refund) {
        TicketOffice office(20.0, 100);
        Visitor v(1, "A", 30, "a@a.by", 100);
        Visitor other(2, "B", 30, "b@a.by", 100);
        Ticket t = office.sell(v, TicketType::Adult, today().addDays(3));
        CHECK_THROW(office.refund(t, other, today()), InvalidTicketException);
        office.refund(t, v, today());
        CHECK_CLOSE(100.0, v.balance(), 1e-9);
        CHECK_CLOSE(0.0, office.revenue(), 1e-9);
        CHECK_EQUAL(0, office.soldOn(today().addDays(3)));
        CHECK_EQUAL(1, office.refundsCount());
        CHECK_THROW(office.refund(t, v, today()), InvalidTicketException);
    }
}

SUITE(ShopTests) {
    TEST(ProductSellRestock) {
        Product p("Plush shark", "Toys", 15.0, 3);
        CHECK_CLOSE(30.0, p.sell(2), 1e-9);
        CHECK_EQUAL(1, p.stock());
        CHECK_THROW(p.sell(2), OutOfStockException);
        p.restock(5);
        CHECK_EQUAL(6, p.stock());
        p.applyMarkdown(20.0);
        CHECK_CLOSE(12.0, p.price(), 1e-9);
        CHECK_THROW(p.applyMarkdown(100.0), InvalidParameterException);
    }

    TEST(GiftShopSales) {
        GiftShop shop("Souvenirs");
        shop.addProduct(Product("Plush shark", "Toys", 15.0, 10));
        shop.addProduct(Product("Magnet", "Gifts", 3.0, 2));
        CHECK_THROW(shop.addProduct(Product("Magnet", "Gifts", 3.0, 2)),
                    InvalidParameterException);
        Visitor v(1, "A", 30, "a@a.by", 100);
        CHECK_CLOSE(30.0, shop.sell("Plush shark", 2, v), 1e-9);
        shop.sell("Magnet", 1, v);
        CHECK_EQUAL("Plush shark", shop.bestSeller());
        CHECK_CLOSE(33.0, shop.revenue(), 1e-9);
        CHECK_EQUAL(2, shop.transactions());
        CHECK_CLOSE(8 * 15.0 + 3.0, shop.inventoryValue(), 1e-9);
        CHECK_EQUAL(1, static_cast<int>(shop.lowStock(1).size()));
        CHECK_THROW(shop.sell("Magnet", 5, v), OutOfStockException);
        CHECK_THROW(shop.find("Hat"), OutOfStockException);
    }

    TEST(GiftShopInsufficientFunds) {
        GiftShop shop("Souvenirs");
        shop.addProduct(Product("Plush shark", "Toys", 15.0, 10));
        Visitor v(1, "A", 30, "a@a.by", 10);
        CHECK_THROW(shop.sell("Plush shark", 1, v), InsufficientFundsException);
        CHECK_EQUAL(10, shop.find("Plush shark").stock());
        CHECK_EQUAL("", shop.bestSeller());
    }
}

SUITE(TourShowTests) {
    TEST(TourGroup) {
        Tour tour(1, TimeSlot(600, 660), 2);
        Visitor a(1, "A", 30, "a@a.by", 0);
        Visitor b(2, "B", 30, "b@a.by", 0);
        Visitor c(3, "C", 30, "c@a.by", 0);
        tour.addVisitor(a);
        CHECK_THROW(tour.addVisitor(a), InvalidParameterException);
        tour.addVisitor(b);
        CHECK_THROW(tour.addVisitor(c), EventFullException);
        CHECK(tour.removeVisitor(1));
        CHECK(!tour.removeVisitor(1));
        CHECK_EQUAL(1, tour.groupSize());
    }

    TEST(TourRoute) {
        Tour tour(1, TimeSlot(600, 630), 10);
        Tank a(1, "A", 5000, seaWater(), hall(), 5);
        Tank b(2, "B", 5000, seaWater(), hall(), 5);
        Tank closed(3, "C", 5000, seaWater(), hall(), 5);
        closed.setOpen(false);
        tour.addStop(&a);
        tour.addStop(&b);
        CHECK_THROW(tour.addStop(&closed), InvalidParameterException);
        CHECK_EQUAL(30, tour.estimatedDurationMinutes());
        CHECK(tour.fitsInSlot());
        tour.addStop(&a);
        CHECK(!tour.fitsInSlot());
        CHECK_THROW(tour.complete(), InvalidParameterException);  // нет гида
    }

    TEST(ShowBooking) {
        Show show("Seal show", TimeSlot(720, 780), 10);
        show.bookSeats(6);
        CHECK_CLOSE(0.6, show.occupancyRate(), 1e-9);
        CHECK_THROW(show.bookSeats(5), EventFullException);
        show.bookSeats(4);
        CHECK(show.isFull());
        show.releaseSeats(2);
        CHECK_EQUAL(8, show.seatsTaken());
        CHECK_THROW(show.releaseSeats(20), InvalidParameterException);
        show.cancel();
        CHECK_THROW(show.bookSeats(1), InvalidParameterException);
        CHECK_THROW(show.addPerformer(nullptr), InvalidParameterException);
    }
}
