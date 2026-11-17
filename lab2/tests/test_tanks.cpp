#include <UnitTest++/UnitTest++.h>

#include <memory>

#include "Exceptions.h"
#include "Tanks.h"
#include "TestHelpers.h"

using namespace ocean;
using namespace testutil;

namespace {
std::shared_ptr<Fish> makeFish(int id, double weight = 1.0) {
    return std::make_shared<Fish>(id, "Fish" + std::to_string(id), clownfish(), 2, weight, 9.0,
                                  true);
}
std::shared_ptr<Shark> makeShark(int id, double weight = 25.0) {
    return std::make_shared<Shark>(id, "Shark" + std::to_string(id), shark(), 8, weight, 150.0,
                                   1200.0);
}
}  // namespace

SUITE(TankTests) {
    TEST(InvalidTankThrows) {
        CHECK_THROW(Tank(0, "T", 1000, seaWater(), hall(), 5), InvalidParameterException);
        CHECK_THROW(Tank(1, "T", 0, seaWater(), hall(), 5), InvalidParameterException);
        CHECK_THROW(Tank(1, "T", 1000, seaWater(), hall(), 0), InvalidParameterException);
    }

    TEST(AddFindRemove) {
        Tank t(1, "Reef", 10000, seaWater(), hall(), 5);
        t.addAnimal(makeFish(1));
        t.addAnimal(makeFish(2));
        CHECK_EQUAL(2, t.animalCount());
        CHECK(t.hasAnimal(1));
        CHECK_EQUAL("Fish2", t.findAnimal(2).name());
        auto removed = t.removeAnimal(1);
        CHECK_EQUAL(1, removed->id());
        CHECK_THROW(t.removeAnimal(1), AnimalNotFoundException);
        CHECK_THROW(t.findAnimal(99), AnimalNotFoundException);
    }

    TEST(AddValidation) {
        Tank t(1, "Reef", 10000, seaWater(), hall(), 2);
        t.addAnimal(makeFish(1));
        CHECK_THROW(t.addAnimal(makeFish(1)), InvalidParameterException);
        CHECK_THROW(t.addAnimal(nullptr), InvalidParameterException);
        CHECK_THROW(t.addAnimal(makeShark(5)), IncompatibleSpeciesException);
        t.addAnimal(makeFish(2));
        CHECK_THROW(t.addAnimal(makeFish(3)), TankCapacityExceededException);
    }

    TEST(WrongWaterThrows) {
        Tank cold(1, "Arctic", 10000, coldWater(), hall(), 5);
        CHECK_THROW(cold.addAnimal(makeFish(1)), InvalidWaterParametersException);
    }

    TEST(Statistics) {
        Tank t(1, "Reef", 2000, seaWater(), hall(), 5);
        t.addAnimal(makeFish(1, 2.0));
        t.addAnimal(std::make_shared<Turtle>(2, "T", turtle(), 10, 8.0, 40.0));
        CHECK_CLOSE(10.0, t.totalBiomass(), 1e-9);
        CHECK_CLOSE(5.0, t.stockingDensity(), 1e-9);
        CHECK_CLOSE(0.06 + 0.12, t.dailyFoodRequirement(), 1e-9);
        CHECK_EQUAL(2, t.speciesCount());
        CHECK_EQUAL(0, t.hungryAnimals());
    }

    TEST(LifeSupportAndCleaning) {
        Tank t(1, "Reef", 1000, seaWater(), hall(), 5);
        t.addAnimal(makeFish(1, 5.0));
        t.water().addAmmonia(0.4);
        CHECK(t.needsCleaning());
        t.performWaterChange(0.5);
        CHECK_CLOSE(0.2, t.water().ammonia(), 1e-9);
        t.runLifeSupport(4.0);
        CHECK(!t.needsCleaning());
        CHECK(t.isHealthy());
        CHECK_THROW(t.performWaterChange(0.9), InvalidParameterException);
        CHECK_THROW(t.runLifeSupport(0.0), InvalidParameterException);
    }

    TEST(LifeSupportFailsWithBrokenFilter) {
        Tank t(1, "Reef", 1000, seaWater(), hall(), 5);
        t.filtration().setActive(false);
        CHECK_THROW(t.runLifeSupport(1.0), EquipmentFailureException);
    }
}

SUITE(SpecialTankTests) {
    TEST(CoralReefRejectsBigShark) {
        CoralReefTank reef(1, "Reef", 50000, seaWater(), hall(), 10, 3.0);
        CHECK(!reef.canHost(*makeShark(1, 50.0)));
        CHECK(reef.canHost(*makeShark(2, 20.0)));
        CHECK_THROW(CoralReefTank(2, "R", 100, seaWater(), hall(), 1, 11.0),
                    InvalidParameterException);
    }

    TEST(CoralReefCorals) {
        CoralReefTank reef(1, "Reef", 50000, seaWater(), hall(), 10, 3.0);
        reef.addCoral(Coral("Acropora", 100.0));
        reef.addCoral(Coral("Porites", 50.0, 40.0));
        CHECK_CLOSE(150.0, reef.totalCoralArea(), 1e-9);
        CHECK_EQUAL(1, reef.bleachedCount());
        reef.growCorals();
        CHECK_CLOSE(152.0, reef.totalCoralArea(), 1e-9);
        CHECK(!reef.bleachingRisk());
        reef.water().adjustTemperature(4.0);
        CHECK(reef.bleachingRisk());
    }

    TEST(OpenOcean) {
        OpenOceanTank big(1, "Ocean", 1e6, seaWater(), hall(), 20, 8.0, 30.0);
        CHECK(big.hasTunnel());
        CHECK_EQUAL(90, big.tunnelVisitorCapacity());
        CHECK(big.requiresDiverCertification());
        big.addAnimal(makeShark(1));
        big.addAnimal(makeShark(2));
        CHECK_EQUAL(2, big.predatorCount());
        OpenOceanTank shallow(2, "Lagoon", 1e5, seaWater(), hall(), 5, 3.0, 0.0);
        CHECK(!shallow.hasTunnel());
        CHECK(!shallow.requiresDiverCertification());
    }

    TEST(TouchPoolHosting) {
        TouchPool pool(1, "Touch", 3000, seaWater(), hall(), 10, 2);
        auto ray = std::make_shared<Ray>(1, "Sting", clownfish(), 3, 4.0, 50.0, true);
        CHECK(!pool.canHost(*ray));
        CHECK_THROW(pool.addAnimal(ray), IncompatibleSpeciesException);
        ray->clipBarb();
        pool.addAnimal(ray);
        CHECK(pool.canHost(Turtle(2, "T", turtle(), 5, 10.0, 30.0)));
        CHECK(!pool.canHost(*makeFish(3)));
        CHECK(!pool.canHost(Jellyfish(4, "J", clownfish(), 1, 0.1, 10.0, true)));
    }

    TEST(TouchPoolLimits) {
        TouchPool pool(1, "Touch", 3000, seaWater(), hall(), 10, 2);
        CHECK(pool.allowTouch());
        CHECK(pool.allowTouch());
        CHECK(!pool.allowTouch());
        CHECK_EQUAL(0, pool.remainingTouches());
        pool.resetDay();
        pool.setOpen(false);
        CHECK(!pool.allowTouch());
    }

    TEST(Quarantine) {
        QuarantineTank q(1, "Q", 2000, seaWater(), hall(), 5, 14);
        CHECK(!q.isOpen());
        auto fish = makeFish(1);
        q.admit(fish, today());
        CHECK(fish->health().isQuarantined());
        CHECK_EQUAL(4, q.daysRemaining(1, today().addDays(10)));
        CHECK(!q.canRelease(1, today().addDays(10)));
        CHECK_THROW(q.release(1, today().addDays(10)), QuarantineViolationException);
        auto released = q.release(1, today().addDays(14));
        CHECK(!released->health().isQuarantined());
        CHECK_EQUAL(0, q.animalCount());
        CHECK_THROW(q.daysRemaining(1, today()), AnimalNotFoundException);
    }

    TEST(QuarantineSeriousConditionBlocksRelease) {
        QuarantineTank q(1, "Q", 2000, seaWater(), hall(), 5, 7);
        auto fish = makeFish(1);
        q.admit(fish, today());
        fish->health().addEntry(MedicalEntry(today(), "infection", "", "Dr", 5));
        CHECK(!q.canRelease(1, today().addDays(30)));
    }
}

SUITE(ExhibitTests) {
    TEST(ExhibitStatistics) {
        Tank a(1, "A", 5000, seaWater(), hall(), 5);
        Tank b(2, "B", 5000, seaWater(), hall(), 5);
        a.addAnimal(makeFish(1));
        b.addAnimal(makeFish(2));
        b.addAnimal(std::make_shared<Turtle>(3, "T", turtle(), 5, 10.0, 30.0));
        Exhibit e("Tropics", "Warm seas");
        e.addTank(&a);
        e.addTank(&b);
        CHECK_EQUAL(3, e.totalAnimals());
        CHECK_EQUAL(2, e.speciesDiversity());
        CHECK_THROW(e.addTank(&a), InvalidParameterException);
        CHECK_THROW(e.addTank(nullptr), InvalidParameterException);
        CHECK(e.removeTank(2));
        CHECK(!e.removeTank(2));
    }

    TEST(ExhibitVisits) {
        Tank a(1, "A", 5000, seaWater(), hall(), 5);
        Exhibit e("Tropics", "Warm seas");
        e.addTank(&a);
        e.registerVisit();
        CHECK_EQUAL(1, e.visitorCount());
        a.setOpen(false);
        CHECK(!e.isOpen());
        CHECK_THROW(e.registerVisit(), OceanariumException);
    }
}

SUITE(MonitorTests) {
    TEST(MonitorRaisesAlerts) {
        Tank t(1, "Reef", 5000, seaWater(), hall(), 5);
        EnvironmentMonitor m;
        m.addSensor(Sensor("T", SensorType::Temperature, 24.0, 27.0));
        m.addSensor(Sensor("NH3", SensorType::Ammonia, 0.0, 0.1));
        CHECK_EQUAL(0, m.check(t));
        t.water().addAmmonia(0.3);
        CHECK_EQUAL(2, m.check(t));  // датчик аммиака + требуется уборка
        CHECK_EQUAL(2, m.alertCount());
        CHECK_EQUAL(2, m.checksPerformed());
        m.clearAlerts();
        CHECK_EQUAL(0, m.alertCount());
    }

    TEST(MonitorCalibration) {
        Tank t(1, "Reef", 5000, seaWater(), hall(), 5);
        EnvironmentMonitor m;
        m.addSensor(Sensor("T", SensorType::Temperature, 24.0, 27.0, false));
        CHECK_THROW(m.check(t), SensorNotCalibratedException);
        m.calibrateAll();
        CHECK_EQUAL(0, m.check(t));
    }
}
