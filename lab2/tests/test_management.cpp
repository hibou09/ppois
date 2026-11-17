#include <UnitTest++/UnitTest++.h>

#include <memory>

#include "Exceptions.h"
#include "Management.h"
#include "TestHelpers.h"

using namespace ocean;
using namespace testutil;

namespace {
Date hired() { return Date(1, 1, 2020); }

std::unique_ptr<Tank> makeTank(int id, int capacity = 10) {
    return std::make_unique<Tank>(id, "Tank" + std::to_string(id), 10000, seaWater(), hall(),
                                  capacity);
}
}  // namespace

SUITE(MaintenanceTests) {
    TEST(TaskLifecycle) {
        Tank t(1, "T", 5000, seaWater(), hall(), 5);
        MaintenanceTask task(1, "Clean glass", &t, 2, today());
        Employee e(1, "E", 1000.0, hired());
        CHECK(!task.isOverdue(today()));
        CHECK(task.isOverdue(today().addDays(1)));
        task.escalate();
        CHECK_EQUAL(3, task.priority());
        task.complete(e);
        CHECK(task.isDone());
        CHECK_EQUAL("E", task.completedBy());
        CHECK(!task.isOverdue(today().addDays(1)));
        CHECK_THROW(task.complete(e), InvalidParameterException);
    }

    TEST(TaskValidation) {
        CHECK_THROW(MaintenanceTask(1, "", nullptr, 2, today()), InvalidParameterException);
        CHECK_THROW(MaintenanceTask(1, "x", nullptr, 9, today()), InvalidParameterException);
        MaintenanceTask task(1, "x", nullptr, 2, today());
        Employee e(1, "E", 1000.0, hired());
        e.dismiss();
        CHECK_THROW(task.complete(e), EmployeeNotQualifiedException);
    }

    TEST(LogCounters) {
        Tank t(1, "T", 5000, seaWater(), hall(), 5);
        MaintenanceLog log;
        int a = log.schedule("Clean", &t, 5, today());
        log.schedule("Check pumps", &t, 2, today().addDays(5));
        log.schedule("Paint", nullptr, 1, today().addDays(-3));
        CHECK_EQUAL(3, log.pendingCount());
        CHECK_EQUAL(1, log.highPriorityPending());
        CHECK_EQUAL(1, log.overdueCount(today()));
        CHECK_EQUAL(2, log.tasksForTank(1));
        Employee e(1, "E", 1000.0, hired());
        log.complete(a, e);
        CHECK_EQUAL(2, log.pendingCount());
        CHECK_THROW(log.complete(42, e), TaskNotFoundException);
    }
}

SUITE(BudgetTests) {
    TEST(IncomeAndExpenses) {
        Budget b(1000.0, 5000.0);
        b.addIncome(500.0);
        b.spend("food", 300.0);
        b.spend("food", 200.0);
        CHECK_CLOSE(1000.0, b.balance(), 1e-9);
        CHECK_CLOSE(500.0, b.spentOn("food"), 1e-9);
        CHECK_CLOSE(0.0, b.spentOn("salary"), 1e-9);
        CHECK_CLOSE(0.0, b.profit(), 1e-9);
        CHECK_CLOSE(4500.0, b.remainingLimit(), 1e-9);
    }

    TEST(BudgetErrors) {
        Budget b(1000.0, 1500.0);
        CHECK_THROW(b.spend("equipment", 1200.0), InsufficientFundsException);
        b.addIncome(5000.0);
        CHECK_THROW(b.spend("equipment", 1600.0), BudgetExceededException);
        CHECK_THROW(b.spend("", 10.0), InvalidParameterException);
        CHECK_THROW(b.addIncome(-1.0), InvalidParameterException);
    }

    TEST(PayrollPaysActiveOnly) {
        Budget b(10000.0, 100000.0);
        Payroll p(10.0);
        Employee a(1, "A", 1000.0, hired());
        Veterinarian v(2, "V", 1000.0, hired(), "L", Date(1, 1, 2030), "x");
        Employee fired(3, "F", 1000.0, hired());
        fired.dismiss();
        CHECK_CLOSE(1430.0, p.calculate(v), 1e-9);
        double paid = p.payAll({&a, &v, &fired}, b);
        CHECK_CLOSE(1100.0 + 1430.0, paid, 1e-9);
        CHECK_EQUAL(2, p.paymentsCount());
        CHECK_CLOSE(paid, b.spentOn("salary"), 1e-9);
        CHECK_THROW(p.setBonus(-5.0), InvalidParameterException);
    }

    TEST(PayrollFailsAtomically) {
        Budget b(100.0, 100000.0);
        Payroll p;
        Employee a(1, "A", 1000.0, hired());
        CHECK_THROW(p.payAll({&a}, b), InsufficientFundsException);
        CHECK_EQUAL(0, p.paymentsCount());
        CHECK_CLOSE(100.0, b.balance(), 1e-9);
    }
}

SUITE(OceanariumTests) {
    TEST(TanksAndEmployees) {
        Oceanarium o("Blue Planet", "Minsk", 100000.0, 25.0);
        o.addTank(makeTank(1));
        CHECK_THROW(o.addTank(makeTank(1)), InvalidParameterException);
        CHECK_EQUAL(1, o.tankCount());
        CHECK_THROW(o.findTank(7), TankNotFoundException);
        o.hire(std::make_unique<Employee>(1, "E", 1000.0, hired()));
        CHECK_THROW(o.hire(std::make_unique<Employee>(1, "E", 1000.0, hired())),
                    InvalidParameterException);
        CHECK_THROW(o.findEmployee(9), EmployeeNotFoundException);
        o.schedule().addShift(Shift(1, today(), TimeSlot(480, 960)));
        o.fire(1);
        CHECK(!o.findEmployee(1).isActive());
        CHECK_CLOSE(0.0, o.schedule().totalHours(1), 1e-9);
    }

    TEST(LocateAndCount) {
        Oceanarium o("Blue Planet", "Minsk", 100000.0, 25.0);
        Tank& t = o.addTank(makeTank(1));
        t.addAnimal(std::make_shared<Fish>(1, "Nemo", clownfish(), 2, 1.0, 9.0, true));
        t.addAnimal(std::make_shared<Turtle>(2, "T", turtle(), 10, 50.0, 60.0));
        CHECK_EQUAL(2, o.totalAnimals());
        CHECK_EQUAL(1, o.endangeredAnimals());
        CHECK_EQUAL(1, o.locateAnimal(2).id());
        CHECK_THROW(o.locateAnimal(99), AnimalNotFoundException);
        CHECK_CLOSE(0.03 + 0.75, o.dailyFoodRequirement(), 1e-9);
    }

    TEST(TransferAnimal) {
        Oceanarium o("Blue Planet", "Minsk", 100000.0, 25.0);
        Tank& a = o.addTank(makeTank(1));
        Tank& b = o.addTank(makeTank(2));
        a.addAnimal(std::make_shared<Fish>(1, "Nemo", clownfish(), 2, 1.0, 9.0, true));
        o.transferAnimal(1, 1, 2);
        CHECK(!a.hasAnimal(1));
        CHECK(b.hasAnimal(1));
        CHECK_THROW(o.transferAnimal(1, 2, 2), InvalidParameterException);
        CHECK_THROW(o.transferAnimal(1, 1, 2), AnimalNotFoundException);
    }

    TEST(TransferRollsBackOnFailure) {
        Oceanarium o("Blue Planet", "Minsk", 100000.0, 25.0);
        Tank& a = o.addTank(makeTank(1));
        Tank& b = o.addTank(makeTank(2));
        a.addAnimal(std::make_shared<Fish>(1, "Nemo", clownfish(), 2, 1.0, 9.0, true));
        b.addAnimal(std::make_shared<Shark>(2, "Bruce", shark(), 8, 25.0, 150.0, 1200.0));
        CHECK_THROW(o.transferAnimal(1, 1, 2), IncompatibleSpeciesException);
        CHECK(a.hasAnimal(1));
        CHECK(!b.hasAnimal(1));
    }

    TEST(FeedingThroughOceanarium) {
        Oceanarium o("Blue Planet", "Minsk", 100000.0, 25.0);
        Tank& t = o.addTank(makeTank(1));
        t.addAnimal(std::make_shared<Fish>(1, "Nemo", clownfish(), 2, 1.0, 9.0, true));
        o.foodStorage().addItem(FoodItem("Krill", 900, 10, 5, Date(1, 1, 2027)));
        o.feeding().addEntry(FeedingEntry(1, "Krill", 0.02, 540));
        CHECK_EQUAL(1, o.feedAnimals(545, today()));
        CHECK_CLOSE(9.98, o.foodStorage().find("Krill").stockKg(), 1e-9);
    }

    TEST(DailyOperationsCreateTasks) {
        Oceanarium o("Blue Planet", "Minsk", 100000.0, 25.0);
        Tank& t = o.addTank(makeTank(1));
        auto fish = std::make_shared<Fish>(1, "Nemo", clownfish(), 2, 1.0, 9.0, true);
        t.addAnimal(fish);
        o.addTank(makeTank(2)).filtration().setActive(false);
        int created = o.runDailyOperations(today());
        CHECK_EQUAL(1, created);  // ремонт фильтра во втором аквариуме
        CHECK_EQUAL(1, o.maintenance().highPriorityPending());
        CHECK_CLOSE(40.0, fish->hunger(), 1e-9);
    }

    TEST(PaySalariesAndSummary) {
        Oceanarium o("Blue Planet", "Minsk", 100000.0, 25.0);
        o.hire(std::make_unique<Aquarist>(1, "A", 1000.0, hired(), 2));
        o.hire(std::make_unique<Employee>(2, "E", 500.0, hired()));
        CHECK_CLOSE(1600.0, o.paySalaries(), 1e-9);
        CHECK_CLOSE(98400.0, o.budget().balance(), 1e-9);
        CHECK_EQUAL("Blue Planet (Minsk): 0 tanks, 0 animals, 2 employees", o.summary());
    }

    TEST(Exhibits) {
        Oceanarium o("Blue Planet", "Minsk", 100000.0, 25.0);
        Tank& t = o.addTank(makeTank(1));
        Exhibit& e = o.addExhibit(Exhibit("Tropics", "Warm seas"));
        e.addTank(&t);
        CHECK_EQUAL(1, static_cast<int>(o.exhibits().size()));
        CHECK(o.exhibits()[0].isOpen());
    }
}

SUITE(ExceptionHierarchyTests) {
    TEST(AllDeriveFromBase) {
        CHECK_THROW(throw TankNotFoundException(1), OceanariumException);
        CHECK_THROW(throw InsufficientFundsException(10, 5), OceanariumException);
        CHECK_THROW(throw QuarantineViolationException("x"), std::runtime_error);
    }
}
