#include <UnitTest++/UnitTest++.h>

#include <memory>
#include <vector>

#include "Exceptions.h"
#include "Feeding.h"
#include "Food.h"
#include "Health.h"
#include "TestHelpers.h"

using namespace ocean;
using namespace testutil;

SUITE(MedicalTests) {
    TEST(EntryValidationAndSeriousness) {
        MedicalEntry e(today(), "fin rot", "antibiotic", "Dr. Fish", 4);
        CHECK(e.isSerious());
        CHECK(!MedicalEntry(today(), "checkup", "", "Dr", 1).isSerious());
        CHECK_THROW(MedicalEntry(today(), "", "", "Dr", 1), InvalidParameterException);
        CHECK_THROW(MedicalEntry(today(), "x", "", "Dr", 6), InvalidParameterException);
    }

    TEST(SummaryContainsDiagnosis) {
        MedicalEntry e(today(), "fin rot", "antibiotic", "Dr. Fish", 2);
        CHECK(e.summary().find("fin rot") != std::string::npos);
    }

    TEST(RecordCheckups) {
        HealthRecord r;
        CHECK(r.needsCheckup(today()));
        CHECK_EQUAL("", r.lastDiagnosis());
        r.addEntry(MedicalEntry(today(), "healthy", "", "Dr", 1));
        CHECK(!r.needsCheckup(today().addDays(100)));
        CHECK(r.needsCheckup(today().addDays(200)));
        CHECK_EQUAL("healthy", r.lastDiagnosis());
    }

    TEST(RecordChronologyAndSerious) {
        HealthRecord r;
        r.addEntry(MedicalEntry(today(), "injury", "", "Dr", 5));
        CHECK(r.hasSeriousCondition());
        CHECK_THROW(r.addEntry(MedicalEntry(today().addDays(-1), "x", "", "Dr", 1)),
                    InvalidDateException);
    }

    TEST(Vaccination) {
        HealthRecord r;
        CHECK(r.vaccinationExpired(today()));
        r.vaccinate(today());
        CHECK(r.isVaccinated());
        CHECK(!r.vaccinationExpired(today().addDays(30)));
        CHECK(r.vaccinationExpired(today().addDays(400)));
    }

    TEST(MedicationDispense) {
        Fish f(1, "Nemo", clownfish(), 2, 2.0, 9.0, true);
        Medication m("Antibiotic", 5.0, 15.0, Date(1, 1, 2027));
        CHECK_CLOSE(10.0, m.doseFor(f), 1e-9);
        CHECK_CLOSE(10.0, m.dispense(f, today()), 1e-9);
        CHECK_CLOSE(5.0, m.stockMg(), 1e-9);
        CHECK_THROW(m.dispense(f, today()), OutOfStockException);
        m.restock(100.0);
        CHECK_CLOSE(105.0, m.stockMg(), 1e-9);
    }

    TEST(MedicationExpired) {
        Fish f(1, "Nemo", clownfish(), 2, 2.0, 9.0, true);
        Medication m("Old", 5.0, 100.0, Date(1, 1, 2025));
        CHECK(m.isExpired(today()));
        CHECK_THROW(m.dispense(f, today()), ExpiredProductException);
    }
}

SUITE(FoodTests) {
    TEST(FoodItemBasics) {
        FoodItem f("Krill", 900, 10, 5, Date(21, 11, 2026));
        CHECK_EQUAL(10, f.daysUntilExpiry(today()));
        CHECK_CLOSE(25.0, f.cost(5.0), 1e-9);
        f.addStock(2.0);
        CHECK_CLOSE(12.0, f.stockKg(), 1e-9);
        CHECK_THROW(f.addStock(0.0), InvalidParameterException);
        CHECK_THROW(FoodItem("", 1, 1, 1, today()), InvalidParameterException);
    }

    TEST(StorageMergesAndCapacity) {
        FoodStorage s(20.0);
        s.addItem(FoodItem("Krill", 900, 10, 5, Date(1, 1, 2027)));
        s.addItem(FoodItem("Krill", 900, 5, 5, Date(1, 1, 2027)));
        CHECK_EQUAL(1, static_cast<int>(s.items().size()));
        CHECK_CLOSE(15.0, s.totalStockKg(), 1e-9);
        CHECK_CLOSE(5.0, s.freeCapacityKg(), 1e-9);
        CHECK_THROW(s.addItem(FoodItem("Squid", 800, 10, 8, Date(1, 1, 2027))),
                    StorageFullException);
    }

    TEST(StorageTakeAndFind) {
        FoodStorage s(100.0);
        s.addItem(FoodItem("Krill", 900, 10, 5, Date(1, 1, 2027)));
        CHECK(s.contains("Krill"));
        s.take("Krill", 4.0, today());
        CHECK_CLOSE(6.0, s.find("Krill").stockKg(), 1e-9);
        CHECK_CLOSE(30.0, s.totalValue(), 1e-9);
        CHECK_THROW(s.find("Tuna"), InsufficientFoodException);
    }

    TEST(StorageRemovesExpired) {
        FoodStorage s(100.0);
        s.addItem(FoodItem("Old", 900, 10, 5, Date(1, 1, 2025)));
        s.addItem(FoodItem("Fresh", 900, 10, 5, Date(1, 1, 2027)));
        CHECK_EQUAL(1, s.removeExpired(today()));
        CHECK(!s.contains("Old"));
    }
}

SUITE(FeedingTests) {
    TEST(EntryDue) {
        FeedingEntry e(1, "Krill", 0.1, 540);
        CHECK(e.isDueAt(550));
        CHECK(!e.isDueAt(600));
        CHECK_THROW(FeedingEntry(1, "Krill", 0.1, 2000), InvalidParameterException);
        CHECK_THROW(FeedingEntry(1, "", 0.1, 60), InvalidParameterException);
    }

    TEST(ScheduleConflict) {
        FeedingSchedule s;
        s.addEntry(FeedingEntry(1, "Krill", 0.1, 540));
        CHECK_THROW(s.addEntry(FeedingEntry(1, "Krill", 0.1, 570)), ScheduleConflictException);
        s.addEntry(FeedingEntry(2, "Krill", 0.2, 540));
        s.addEntry(FeedingEntry(1, "Squid", 0.1, 900));
        CHECK_EQUAL(2, static_cast<int>(s.entriesFor(1).size()));
        CHECK_EQUAL(2, static_cast<int>(s.dueAt(540).size()));
        CHECK_CLOSE(0.3, s.dailyAmount("Krill"), 1e-9);
    }

    TEST(ExecuteFeedsAnimals) {
        FeedingSchedule s;
        s.addEntry(FeedingEntry(1, "Krill", 0.03, 540));
        FoodStorage storage(100.0);
        storage.addItem(FoodItem("Krill", 900, 10, 5, Date(1, 1, 2027)));
        auto fish = std::make_shared<Fish>(1, "Nemo", clownfish(), 2, 1.0, 9.0, true);
        fish->passDay();
        fish->passDay();
        std::vector<std::shared_ptr<Animal>> animals{fish};
        CHECK_EQUAL(1, s.execute(540, animals, storage, today()));
        CHECK(!fish->isHungry());
        CHECK_EQUAL(0, s.execute(800, animals, storage, today()));
        CHECK_EQUAL(1, s.executedCount());
    }

    TEST(Supplier) {
        Supplier sup("SeaFood Ltd", 3, 10.0);
        CHECK_CLOSE(90.0, sup.quote(10.0, 10.0), 1e-9);
        CHECK(sup.deliveryDate(today()) == Date(14, 11, 2026));
        FoodItem f = sup.deliver("Krill", 10.0, 10.0, 900, today(), 30);
        CHECK_CLOSE(9.0, f.pricePerKg(), 1e-9);
        CHECK(f.expiry() == Date(14, 12, 2026));
        sup.rate(5);
        sup.rate(3);
        CHECK_CLOSE(4.0, sup.rating(), 1e-9);
        CHECK_THROW(sup.rate(6), InvalidParameterException);
    }
}
