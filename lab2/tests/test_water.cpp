#include <UnitTest++/UnitTest++.h>

#include "Exceptions.h"
#include "TestHelpers.h"
#include "Water.h"

using namespace ocean;

SUITE(WaterParametersTests) {
    TEST(ConstructionValidates) {
        CHECK_THROW(WaterParameters(25, 35, 15, 7), InvalidWaterParametersException);
        CHECK_THROW(WaterParameters(25, -1, 8, 7), InvalidWaterParametersException);
        CHECK_THROW(WaterParameters(25, 35, 8, -1), InvalidWaterParametersException);
        CHECK_THROW(WaterParameters(50, 35, 8, 7), InvalidWaterParametersException);
    }

    TEST(AdjustTemperature) {
        WaterParameters w = testutil::seaWater();
        w.adjustTemperature(1.5);
        CHECK_CLOSE(26.5, w.temperature(), 1e-9);
        CHECK_THROW(w.adjustTemperature(20.0), InvalidWaterParametersException);
    }

    TEST(AmmoniaAddAndReduce) {
        WaterParameters w = testutil::seaWater();
        w.addAmmonia(0.4);
        w.reduceAmmonia(0.5);
        CHECK_CLOSE(0.2, w.ammonia(), 1e-9);
        CHECK_THROW(w.reduceAmmonia(1.5), InvalidParameterException);
        CHECK_THROW(w.addAmmonia(-1.0), InvalidParameterException);
    }

    TEST(MixWith) {
        WaterParameters w(20, 30, 8.0, 6.0, 1.0);
        w.mixWith(WaterParameters(30, 30, 8.0, 6.0, 0.0), 0.5);
        CHECK_CLOSE(25.0, w.temperature(), 1e-9);
        CHECK_CLOSE(0.5, w.ammonia(), 1e-9);
    }

    TEST(ToxicityAndQuality) {
        WaterParameters clean = testutil::seaWater();
        CHECK(!clean.isToxic());
        CHECK(clean.qualityIndex() > 95.0);
        WaterParameters dirty(25, 35, 8.2, 7.0, 0.8);
        CHECK(dirty.isToxic());
        CHECK(dirty.qualityIndex() < clean.qualityIndex());
    }
}

SUITE(WaterRequirementsTests) {
    TEST(Accepts) {
        WaterRequirements r = testutil::tropical();
        CHECK(r.accepts(testutil::seaWater()));
        CHECK(!r.accepts(testutil::coldWater()));
    }

    TEST(InvalidRangeThrows) {
        CHECK_THROW(WaterRequirements(30, 20, 30, 37, 7.8, 8.5, 5), InvalidParameterException);
    }

    TEST(IsSaltwater) {
        CHECK(testutil::tropical().isSaltwater());
        CHECK(!WaterRequirements(20, 26, 0, 1, 6.5, 7.5, 5).isSaltwater());
    }

    TEST(IntersectAndOverlap) {
        WaterRequirements a = testutil::tropical();
        WaterRequirements b(26, 30, 32, 40, 8.0, 8.6, 6);
        WaterRequirements i = a.intersect(b);
        CHECK_CLOSE(26.0, i.minTemp(), 1e-9);
        CHECK_CLOSE(28.0, i.maxTemp(), 1e-9);
        CHECK_CLOSE(6.0, i.minOxygen(), 1e-9);
        CHECK(!a.overlaps(testutil::cold()));
        CHECK_THROW(a.intersect(testutil::cold()), IncompatibleSpeciesException);
    }

    TEST(IdealAndDeviation) {
        WaterRequirements r = testutil::tropical();
        CHECK(r.accepts(r.ideal()));
        CHECK_CLOSE(0.0, r.deviation(testutil::seaWater()), 1e-9);
        CHECK(r.deviation(testutil::coldWater()) > 10.0);
    }
}

SUITE(SensorTests) {
    TEST(ReadAndAlarm) {
        Sensor s("T1", SensorType::Temperature, 24.0, 27.0);
        CHECK_CLOSE(25.0, s.read(testutil::seaWater()), 1e-9);
        CHECK(!s.isAlarm());
        s.read(testutil::coldWater());
        CHECK(s.isAlarm());
    }

    TEST(UncalibratedThrows) {
        Sensor s("PH", SensorType::Ph, 7.8, 8.5, false);
        CHECK_THROW(s.read(testutil::seaWater()), SensorNotCalibratedException);
        s.calibrate();
        CHECK_CLOSE(8.2, s.read(testutil::seaWater()), 1e-9);
    }

    TEST(DriftAfterManyReadings) {
        Sensor s("O2", SensorType::Oxygen, 5.0, 10.0);
        for (int i = 0; i < Sensor::kDriftLimit; ++i) {
            s.read(testutil::seaWater());
        }
        CHECK(!s.isCalibrated());
    }

    TEST(InvalidSensorThrows) {
        CHECK_THROW(Sensor("", SensorType::Ph, 7, 8), InvalidParameterException);
        CHECK_THROW(Sensor("X", SensorType::Ph, 9, 8), InvalidParameterException);
    }
}

SUITE(EquipmentTests) {
    TEST(FiltrationReducesAmmoniaAndWears) {
        FiltrationSystem f("F", 1000.0);
        WaterParameters w(25, 35, 8.2, 7.0, 0.5);
        f.runCycle(w, 2.0);
        CHECK(w.ammonia() < 0.5);
        CHECK_CLOSE(1.0, f.filterWear(), 1e-9);
        CHECK_CLOSE(2.0, f.turnoverPerHour(500.0), 1e-9);
    }

    TEST(FiltrationFailures) {
        FiltrationSystem f("F", 1000.0);
        WaterParameters w = testutil::seaWater();
        f.setActive(false);
        CHECK_THROW(f.runCycle(w, 1.0), EquipmentFailureException);
        f.setActive(true);
        f.runCycle(w, 200.0);
        CHECK(f.needsMaintenance());
        CHECK_THROW(f.runCycle(w, 1.0), EquipmentFailureException);
        f.replaceFilter();
        CHECK(!f.needsMaintenance());
    }

    TEST(HeatingRegulates) {
        HeatingSystem h(2.0, 26.0);
        WaterParameters w(22, 35, 8.2, 7.0);
        h.regulate(w, 1.0);
        CHECK_CLOSE(23.0, w.temperature(), 1e-9);
        h.regulate(w, 10.0);
        CHECK_CLOSE(26.0, w.temperature(), 1e-9);
        CHECK_CLOSE(10.0, h.energyConsumption(5.0), 1e-9);
        CHECK_THROW(h.setTargetTemperature(40.0), InvalidParameterException);
    }

    TEST(HeatingInactiveDoesNothing) {
        HeatingSystem h(2.0, 26.0);
        h.setActive(false);
        WaterParameters w(22, 35, 8.2, 7.0);
        h.regulate(w, 5.0);
        CHECK_CLOSE(22.0, w.temperature(), 1e-9);
        CHECK_CLOSE(0.0, h.energyConsumption(5.0), 1e-9);
    }

    TEST(Lighting) {
        LightingSystem l(400.0, 10.0, 1000.0);
        CHECK(l.isLightOn(9 * 60));
        CHECK(!l.isLightOn(7 * 60));
        CHECK(!l.isLightOn(18 * 60));
        CHECK_CLOSE(4.0, l.dailyEnergyKwh(), 1e-9);
        l.dim(50.0);
        CHECK_CLOSE(500.0, l.intensity(), 1e-9);
        CHECK_THROW(l.setDayLength(20.0), InvalidParameterException);
    }
}
