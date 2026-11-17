/**
 * @file test_edge_cases.cpp
 * @brief Тесты граничных случаев: валидация аргументов и редко выполняемые ветки.
 */
#include <UnitTest++/UnitTest++.h>

#include <memory>

#include "Exceptions.h"
#include "Management.h"
#include "TestHelpers.h"

using namespace ocean;
using namespace testutil;

namespace {
Date hired() { return Date(1, 1, 2020); }
}  // namespace

SUITE(CommonEdgeCases) {
    TEST(DaysInMonthRejectsBadMonth) {
        CHECK_THROW(Date::daysInMonth(0, 2026), InvalidDateException);
        CHECK_THROW(Date::daysInMonth(13, 2026), InvalidDateException);
        CHECK_EQUAL(29, Date::daysInMonth(2, 2028));
        CHECK_EQUAL(28, Date::daysInMonth(2, 2026));
    }
}

SUITE(WaterEdgeCases) {
    TEST(NegativeAmmoniaRejected) {
        CHECK_THROW(WaterParameters(25.0, 35.0, 8.0, 7.0, -0.1), InvalidWaterParametersException);
    }

    TEST(MixFractionOutOfRange) {
        WaterParameters w = seaWater();
        CHECK_THROW(w.mixWith(coldWater(), -0.1), InvalidParameterException);
        CHECK_THROW(w.mixWith(coldWater(), 1.5), InvalidParameterException);
    }

    TEST(LowOxygenLowersQuality) {
        WaterParameters good(25.0, 35.0, 8.1, 7.0);
        WaterParameters poor(25.0, 35.0, 8.1, 3.0);
        CHECK_CLOSE(100.0, good.qualityIndex(), 1e-9);
        CHECK_CLOSE(70.0, poor.qualityIndex(), 1e-9);
    }

    TEST(RequirementsValidation) {
        CHECK_THROW(WaterRequirements(23.0, 28.0, 30.0, 37.0, 7.8, 8.5, -1.0),
                    InvalidParameterException);
    }

    TEST(DeviationCountsLowOxygen) {
        WaterParameters w(25.0, 35.0, 8.0, 2.0);
        CHECK_CLOSE(3.0, tropical().deviation(w), 1e-9);
    }

    TEST(SensorReadsSalinity) {
        Sensor s("sal-1", SensorType::Salinity, 30.0, 37.0);
        CHECK_CLOSE(35.0, s.read(seaWater()), 1e-9);
        CHECK_CLOSE(35.0, s.lastValue(), 1e-9);
        CHECK(!s.isAlarm());
    }

    TEST(FiltrationValidation) {
        FiltrationSystem f("F-100", 1000.0);
        WaterParameters w = seaWater();
        CHECK_THROW(f.runCycle(w, 0.0), InvalidParameterException);
        CHECK_THROW(f.runCycle(w, -1.0), InvalidParameterException);
        CHECK_THROW(f.turnoverPerHour(0.0), InvalidParameterException);
        CHECK_CLOSE(0.5, f.turnoverPerHour(2000.0), 1e-9);
    }

    TEST(HeaterAndLampValidation) {
        CHECK_THROW(HeatingSystem(0.0, 25.0), InvalidParameterException);
        CHECK_THROW(LightingSystem(0.0, 8.0, 500.0), InvalidParameterException);
        CHECK_THROW(LightingSystem(100.0, 8.0, -1.0), InvalidParameterException);
        CHECK_THROW(LightingSystem(100.0, 20.0, 500.0), InvalidParameterException);
    }

    TEST(LampDimRange) {
        LightingSystem lamp(100.0, 8.0, 500.0);
        CHECK_THROW(lamp.dim(-1.0), InvalidParameterException);
        CHECK_THROW(lamp.dim(100.5), InvalidParameterException);
        lamp.dim(50.0);
        CHECK_CLOSE(250.0, lamp.intensity(), 1e-9);
    }
}

SUITE(FoodHealthEdgeCases) {
    TEST(FoodItemValidation) {
        Date exp(1, 1, 2027);
        CHECK_THROW(FoodItem("Krill", 0.0, 10.0, 5.0, exp), InvalidParameterException);
        CHECK_THROW(FoodItem("Krill", 900.0, -1.0, 5.0, exp), InvalidParameterException);
        CHECK_THROW(FoodItem("Krill", 900.0, 10.0, -5.0, exp), InvalidParameterException);
    }

    TEST(FoodItemAmountsMustBePositive) {
        FoodItem item("Krill", 900.0, 10.0, 5.0, Date(1, 1, 2027));
        CHECK_THROW(item.consume(0.0, today()), InvalidParameterException);
        CHECK_THROW(item.consume(-1.0, today()), InvalidParameterException);
        CHECK_THROW(item.cost(-1.0), InvalidParameterException);
        CHECK_CLOSE(10.0, item.cost(2.0), 1e-9);
    }

    TEST(StorageCapacityMustBePositive) {
        CHECK_THROW(FoodStorage(0.0), InvalidParameterException);
        CHECK_THROW(FoodStorage(-5.0), InvalidParameterException);
    }

    TEST(MedicationValidation) {
        Date exp(1, 1, 2027);
        CHECK_THROW(Medication("", 1.0, 100.0, exp), InvalidParameterException);
        CHECK_THROW(Medication("Amox", 0.0, 100.0, exp), InvalidParameterException);
        CHECK_THROW(Medication("Amox", 1.0, -1.0, exp), InvalidParameterException);
        Medication m("Amox", 1.0, 100.0, exp);
        CHECK_THROW(m.restock(0.0), InvalidParameterException);
        CHECK_THROW(m.restock(-3.0), InvalidParameterException);
    }
}

SUITE(AnimalEdgeCases) {
    TEST(AnimalConstructorsValidateArguments) {
        CHECK_THROW(Shark(1, "S", shark(), 5, 20.0, 140.0, 0.0), InvalidParameterException);
        CHECK_THROW(Turtle(2, "T", turtle(), 5, 20.0, 0.0), InvalidParameterException);
        CHECK_THROW(Octopus(3, "O", clownfish(), 3, 5.0, -1), InvalidParameterException);
        CHECK_THROW(Octopus(3, "O", clownfish(), 3, 5.0, 101), InvalidParameterException);
        CHECK_THROW(Jellyfish(4, "J", clownfish(), 1, 1.0, 0.0, false), InvalidParameterException);
        CHECK_THROW(Coral("", 10.0, 100.0), InvalidParameterException);
        CHECK_THROW(Coral("Acropora", 0.0, 100.0), InvalidParameterException);
        CHECK_THROW(Coral("Acropora", 10.0, -1.0), InvalidParameterException);
        CHECK_THROW(Coral("Acropora", 10.0, 100.5), InvalidParameterException);
    }

    TEST(OctopusFoodIsFourPercentOfWeight) {
        Octopus o(1, "Otto", clownfish(), 3, 10.0, 80);
        CHECK_CLOSE(0.4, o.dailyFoodKg(), 1e-9);
    }

    TEST(PenguinSwimNeedsPositiveHours) {
        Penguin p(1, "Pingu", penguin(), 4, 6.0, "North");
        CHECK_THROW(p.swim(0.0), InvalidParameterException);
        CHECK_THROW(p.swim(-2.0), InvalidParameterException);
    }

    TEST(SupplierValidation) {
        CHECK_THROW(Supplier("", 3, 5.0), InvalidParameterException);
        CHECK_THROW(Supplier("S", -1, 5.0), InvalidParameterException);
        CHECK_THROW(Supplier("S", 3, -1.0), InvalidParameterException);
        CHECK_THROW(Supplier("S", 3, 100.0), InvalidParameterException);
        Supplier s("S", 3, 10.0);
        CHECK_THROW(s.quote(0.0, 5.0), InvalidParameterException);
        CHECK_THROW(s.quote(5.0, -1.0), InvalidParameterException);
        CHECK_THROW(s.deliver("Krill", 10.0, 5.0, 900.0, today(), 0), InvalidParameterException);
    }
}

SUITE(TankEdgeCases) {
    TEST(TankSubclassValidation) {
        CHECK_THROW(OpenOceanTank(1, "O", 1000.0, seaWater(), hall(), 5, 0.0, 10.0),
                    InvalidParameterException);
        CHECK_THROW(OpenOceanTank(1, "O", 1000.0, seaWater(), hall(), 5, 5.0, -1.0),
                    InvalidParameterException);
        CHECK_THROW(TouchPool(1, "P", 1000.0, seaWater(), hall(), 5, 0), InvalidParameterException);
        CHECK_THROW(QuarantineTank(1, "Q", 1000.0, seaWater(), hall(), 5, 0),
                    InvalidParameterException);
        CHECK_THROW(Exhibit("", "theme"), InvalidParameterException);
    }

    TEST(TouchPoolRejectsOrdinaryFish) {
        TouchPool pool(1, "Pool", 1000.0, seaWater(), hall(), 5, 10);
        Fish fish(1, "Nemo", clownfish(), 2, 0.25, 9.0, true);
        CHECK(!pool.canHost(fish));
        CHECK_THROW(pool.addAnimal(std::make_shared<Fish>(fish)), IncompatibleSpeciesException);
        Turtle t(2, "Tortilla", turtle(), 30, 120.0, 90.0);
        CHECK(pool.canHost(t));
        TouchPool coldPool(2, "Cold", 1000.0, coldWater(), hall(), 5, 10);
        CHECK(!coldPool.canHost(t));  // подходящий вид, но неподходящая вода
    }
}

SUITE(VisitorEdgeCases) {
    TEST(VisitorAndTicketValidation) {
        CHECK_THROW(Visitor(0, "A", 20, "a@b.c", 10.0), InvalidParameterException);
        CHECK_THROW(Visitor(1, "", 20, "a@b.c", 10.0), InvalidParameterException);
        Visitor v(1, "Anna", 20, "a@b.c", 10.0);
        CHECK_THROW(v.pay(-1.0), InvalidParameterException);
        CHECK_THROW(Ticket(1, TicketType::Adult, -1.0, today(), 1), InvalidParameterException);
    }

    TEST(TicketOfficeValidation) {
        CHECK_THROW(TicketOffice(0.0, 10), InvalidParameterException);
        CHECK_THROW(TicketOffice(10.0, 0), InvalidParameterException);
    }

    TEST(ExpiredPromoCodeBlocksSale) {
        TicketOffice office(20.0, 100);
        Visitor v(1, "Anna", 30, "a@b.c", 100.0);
        PromoCode spent("OLD", 10.0, 0, today().addDays(30));
        CHECK_THROW(office.sell(v, TicketType::Adult, today(), nullptr, &spent),
                    InvalidPromoCodeException);
        CHECK_CLOSE(100.0, v.balance(), 1e-9);
        CHECK_EQUAL(0, office.soldOn(today()));
    }

    TEST(ProductAndShopValidation) {
        CHECK_THROW(Product("", "toys", 5.0, 1), InvalidParameterException);
        CHECK_THROW(Product("Plush", "toys", -5.0, 1), InvalidParameterException);
        CHECK_THROW(Product("Plush", "toys", 5.0, -1), InvalidParameterException);
        Product p("Plush", "toys", 5.0, 10);
        CHECK_THROW(p.sell(0), InvalidParameterException);
        CHECK_THROW(p.restock(0), InvalidParameterException);

        GiftShop shop("Shop");
        shop.addProduct(p);
        Visitor v(1, "Anna", 30, "a@b.c", 100.0);
        CHECK_THROW(shop.sell("Plush", 0, v), InvalidParameterException);
        CHECK_CLOSE(100.0, v.balance(), 1e-9);
    }

    TEST(TourValidation) {
        CHECK_THROW(Tour(1, TimeSlot(600, 660), 0), InvalidParameterException);
        Tour tour(1, TimeSlot(600, 660), 5);
        CHECK_THROW(tour.assignGuide(nullptr), InvalidParameterException);
        CHECK_THROW(tour.addStop(nullptr), InvalidParameterException);
    }

    TEST(TourNeedsStopsAndCannotBeJoinedAfterCompletion) {
        Tour tour(1, TimeSlot(600, 660), 5);
        Guide guide(1, "Gus", 1000.0, hired(), {"en"});
        Tank tank(1, "T", 1000.0, seaWater(), hall(), 5);
        Visitor v(1, "Anna", 30, "a@b.c", 0.0);
        tour.assignGuide(&guide);
        CHECK_THROW(tour.complete(), InvalidParameterException);  // нет остановок
        tour.addStop(&tank);
        tour.complete();
        CHECK(tour.isCompleted());
        CHECK_THROW(tour.addVisitor(v), InvalidParameterException);
    }

    TEST(ShowValidation) {
        CHECK_THROW(Show("", TimeSlot(600, 660), 10), InvalidParameterException);
        CHECK_THROW(Show("Seals", TimeSlot(600, 660), 0), InvalidParameterException);
        Show show("Seals", TimeSlot(600, 660), 10);
        CHECK_THROW(show.bookSeats(0), InvalidParameterException);
        CHECK_THROW(show.bookSeats(-2), InvalidParameterException);
    }
}

SUITE(StaffEdgeCases) {
    TEST(EmployeeSubclassValidation) {
        CHECK_THROW(Employee(0, "E", 1000.0, hired()), InvalidParameterException);
        CHECK_THROW(Employee(1, "", 1000.0, hired()), InvalidParameterException);
        CHECK_THROW(Aquarist(1, "A", 1000.0, hired(), 0), InvalidParameterException);
        CHECK_THROW(Aquarist(1, "A", 1000.0, hired(), 4), InvalidParameterException);
        CHECK_THROW(Veterinarian(1, "V", 1000.0, hired(), "", today().addDays(10), "fish"),
                    InvalidParameterException);
        CHECK_THROW(Diver(1, "D", 1000.0, hired(), 0.0), InvalidParameterException);
    }

    TEST(AquaristNullTankRejected) {
        Aquarist a(1, "A", 1000.0, hired(), 1);
        CHECK_THROW(a.assignTank(nullptr), InvalidParameterException);
    }

    TEST(DismissedAquaristCannotClean) {
        Aquarist a(1, "A", 1000.0, hired(), 1);
        Tank tank(1, "T", 1000.0, seaWater(), hall(), 5);
        a.assignTank(&tank);
        a.dismiss();
        CHECK_THROW(a.cleanTank(tank), EmployeeNotQualifiedException);
    }

    TEST(CleaningReplacesWornFilter) {
        Aquarist a(1, "A", 1000.0, hired(), 1);
        Tank tank(1, "T", 5000.0, seaWater(), hall(), 5);
        tank.filtration().runCycle(tank.water(), 170.0);  // износ 85%
        CHECK(tank.filtration().needsMaintenance());
        a.assignTank(&tank);
        a.cleanTank(tank);
        CHECK(!tank.filtration().needsMaintenance());
        CHECK_CLOSE(0.0, tank.filtration().filterWear(), 1e-9);
    }

    TEST(DismissedVeterinarianCannotExamine) {
        Veterinarian vet(1, "V", 1000.0, hired(), "L-1", today().addDays(100), "fish");
        Fish fish(1, "Nemo", clownfish(), 2, 0.25, 9.0, true);
        vet.dismiss();
        CHECK_THROW(vet.examine(fish, today(), "cold", 1), EmployeeNotQualifiedException);
        CHECK_EQUAL(0, vet.examinations());
    }

    TEST(GuideLanguageMustBeNonEmpty) {
        Guide g(1, "Gus", 1000.0, hired(), {"en"});
        CHECK_THROW(g.learnLanguage(""), InvalidParameterException);
        g.learnLanguage("de");
        CHECK(g.speaks("de"));
    }
}

SUITE(ManagementEdgeCases) {
    TEST(BudgetValidation) {
        CHECK_THROW(Budget(-1.0, 100.0), InvalidParameterException);
        CHECK_THROW(Budget(100.0, 0.0), InvalidParameterException);
    }

    TEST(PayrollSkipsWhenEveryoneDismissed) {
        Employee e(1, "E", 1000.0, hired());
        e.dismiss();
        Budget budget(10000.0, 50000.0);
        Payroll payroll;
        std::vector<Employee*> staff{&e, nullptr};
        CHECK_CLOSE(0.0, payroll.payAll(staff, budget), 1e-9);
        CHECK_EQUAL(0, payroll.paymentsCount());
        CHECK_CLOSE(10000.0, budget.balance(), 1e-9);
    }

    TEST(OceanariumValidation) {
        CHECK_THROW(Oceanarium("", "Minsk", 1000.0, 20.0), InvalidParameterException);
        Oceanarium o("Blue", "Minsk", 1000.0, 20.0);
        CHECK_THROW(o.addTank(nullptr), InvalidParameterException);
        CHECK_THROW(o.hire(nullptr), InvalidParameterException);
    }

    TEST(DailyOperationsSchedulesCleaningAndFilterReplacement) {
        Oceanarium o("Blue", "Minsk", 1000000.0, 20.0);
        Tank& tank = o.addTank(std::make_unique<Tank>(1, "Dirty", 5000.0, seaWater(), hall(), 5));
        tank.filtration().runCycle(tank.water(), 140.0);  // износ 70%, после суток ≥ 80%
        tank.water().addAmmonia(500.0);                    // после фильтрации всё ещё грязно
        int created = o.runDailyOperations(today());
        CHECK_EQUAL(2, created);
        CHECK_EQUAL(2, o.maintenance().pendingCount());
        CHECK_EQUAL(2, o.maintenance().tasksForTank(1));
    }
}
