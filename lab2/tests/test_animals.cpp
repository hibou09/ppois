#include <UnitTest++/UnitTest++.h>

#include "Animals.h"
#include "Exceptions.h"
#include "TestHelpers.h"

using namespace ocean;
using namespace testutil;

SUITE(SpeciesTests) {
    TEST(Endangered) {
        CHECK(turtle().isEndangered());
        CHECK(!clownfish().isEndangered());
    }

    TEST(Coexistence) {
        CHECK(clownfish().canCoexistWith(turtle()));
        CHECK(!clownfish().canCoexistWith(shark()));
        CHECK(shark().canCoexistWith(shark()));
        CHECK(!clownfish().canCoexistWith(penguin()));
    }

    TEST(EmptyNameThrows) {
        CHECK_THROW(Species("", "x", Diet::Omnivore, tropical(), false,
                            ConservationStatus::LeastConcern),
                    InvalidParameterException);
    }
}

SUITE(AnimalTests) {
    TEST(InvalidAnimalThrows) {
        CHECK_THROW(Fish(0, "A", clownfish(), 1, 1.0, 5.0, true), InvalidParameterException);
        CHECK_THROW(Fish(1, "", clownfish(), 1, 1.0, 5.0, true), InvalidParameterException);
        CHECK_THROW(Fish(1, "A", clownfish(), 1, -1.0, 5.0, true), InvalidParameterException);
        CHECK_THROW(Fish(1, "A", clownfish(), 1, 1.0, 0.0, true), InvalidParameterException);
    }

    TEST(HungerAndFeeding) {
        Fish f(1, "Nemo", clownfish(), 2, 1.0, 9.0, true);
        CHECK(!f.isHungry());
        f.passDay();
        f.passDay();
        CHECK(f.isHungry());
        FoodItem krill("Krill", 900, 10, 5, Date(1, 1, 2027));
        f.feed(krill, 0.03, today());  // суточная норма
        CHECK(!f.isHungry());
        CHECK_CLOSE(9.97, krill.stockKg(), 1e-9);
    }

    TEST(FeedingValidation) {
        Fish f(1, "Nemo", clownfish(), 2, 1.0, 9.0, true);
        FoodItem expired("Old", 900, 10, 5, Date(1, 1, 2025));
        CHECK_THROW(f.feed(expired, 0.1, today()), ExpiredProductException);
        FoodItem little("Krill", 900, 0.01, 5, Date(1, 1, 2027));
        CHECK_THROW(f.feed(little, 0.1, today()), InsufficientFoodException);
        CHECK_THROW(f.feed(little, 0.0, today()), InvalidParameterException);
    }

    TEST(OverfeedingIncreasesWeight) {
        Fish f(1, "Nemo", clownfish(), 2, 1.0, 9.0, true);
        FoodItem krill("Krill", 900, 10, 5, Date(1, 1, 2027));
        f.feed(krill, 1.0, today());
        CHECK_CLOSE(1.05, f.weightKg(), 1e-9);
    }

    TEST(BirthdayAndDescribe) {
        Fish f(1, "Nemo", clownfish(), 2, 1.0, 9.0, true);
        f.celebrateBirthday();
        CHECK_EQUAL(3, f.ageYears());
        CHECK_EQUAL("Fish Nemo (Clownfish, 3 y.o.)", f.describe());
    }

    TEST(WaterAndCompanions) {
        Fish nemo(1, "Nemo", clownfish(), 2, 1.0, 9.0, true);
        Shark bruce(2, "Bruce", shark(), 10, 25.0, 160.0, 1300.0);
        CHECK(nemo.canLiveIn(seaWater()));
        CHECK(!nemo.canLiveIn(coldWater()));
        CHECK(!nemo.canShareTankWith(bruce));
    }
}

SUITE(FishFamilyTests) {
    TEST(FishSpeedAndGrowth) {
        Fish f(1, "Nemo", clownfish(), 2, 1.0, 10.0, true);
        CHECK_CLOSE(0.2, f.swimSpeed(), 1e-9);
        f.grow(2.0);
        CHECK_CLOSE(12.0, f.lengthCm(), 1e-9);
        CHECK_THROW(f.grow(-1.0), InvalidParameterException);
        CHECK_CLOSE(0.03, f.dailyFoodKg(), 1e-9);
    }

    TEST(SharkDanger) {
        Shark s(1, "Bruce", shark(), 10, 40.0, 160.0, 1300.0);
        Fish small(2, "Nemo", clownfish(), 2, 1.0, 9.0, true);
        Shark other(3, "Jaws", shark(), 10, 10.0, 120.0, 900.0);
        CHECK(s.isDangerousTo(small));
        CHECK(!s.isDangerousTo(other));
        CHECK(!s.isDangerousTo(s));
        CHECK_EQUAL("Shark", s.kind());
        CHECK_CLOSE(0.8, s.dailyFoodKg(), 1e-9);
    }

    TEST(SharkTeeth) {
        Shark s(1, "Bruce", shark(), 10, 40.0, 160.0, 1300.0);
        s.loseTeeth(3);
        CHECK_EQUAL(3, s.teethLost());
        CHECK_THROW(s.loseTeeth(0), InvalidParameterException);
    }

    TEST(RayTouchSafety) {
        Ray venomous(1, "Sting", clownfish(), 3, 5.0, 60.0, true);
        Ray harmless(2, "Manta", clownfish(), 3, 5.0, 60.0, false);
        CHECK(!venomous.isSafeForTouch());
        venomous.clipBarb();
        CHECK(venomous.isSafeForTouch());
        CHECK(harmless.isSafeForTouch());
        CHECK_THROW(harmless.clipBarb(), InvalidParameterException);
    }
}

SUITE(OtherAnimalsTests) {
    TEST(TurtleBasking) {
        Turtle t(1, "Tortilla", turtle(), 30, 100.0, 80.0);
        CHECK(t.needsBasking());
        t.bask(3.0);
        CHECK(!t.needsBasking());
        CHECK_THROW(t.bask(10.0), InvalidParameterException);
        t.passDay();
        CHECK(t.needsBasking());
        CHECK_EQUAL(50, t.remainingLifespan());
        CHECK_CLOSE(1.5, t.dailyFoodKg(), 1e-9);
    }

    TEST(OctopusPuzzlesAndEscape) {
        Octopus o(1, "Inky", clownfish(), 2, 10.0, 75);
        CHECK(o.solvePuzzle(7));
        CHECK_EQUAL(77, o.intelligence());
        CHECK(!o.solvePuzzle(9));
        CHECK_THROW(o.solvePuzzle(11), InvalidParameterException);
        CHECK(o.attemptEscape(false));
        CHECK(!o.attemptEscape(true));
        CHECK_EQUAL(2, o.escapeAttempts());
    }

    TEST(OctopusArms) {
        Octopus o(1, "Inky", clownfish(), 2, 10.0, 50);
        o.loseArm();
        CHECK_EQUAL(7, o.armCount());
        o.regenerateArm();
        o.regenerateArm();
        CHECK_EQUAL(8, o.armCount());
        for (int i = 0; i < 8; ++i) o.loseArm();
        CHECK_THROW(o.loseArm(), InvalidParameterException);
    }

    TEST(Jellyfish) {
        Jellyfish j(1, "Jelly", clownfish(), 1, 0.2, 20.0, true);
        CHECK(!j.isSafeForTouch());
        CHECK_CLOSE(40.0, j.pulseRate(25.0), 1e-9);
        CHECK_CLOSE(10.0, j.pulseRate(0.0), 1e-9);
        CHECK_CLOSE(0.01, j.dailyFoodKg(), 1e-9);
    }

    TEST(PenguinSwimAndMolt) {
        Penguin p(1, "Pingu", penguin(), 4, 6.0, "Gentoo colony");
        CHECK(!p.hadEnoughExercise());
        p.swim(1.5);
        CHECK(p.hadEnoughExercise());
        CHECK_CLOSE(0.6, p.dailyFoodKg(), 1e-9);
        p.startMolt();
        CHECK_CLOSE(0.9, p.dailyFoodKg(), 1e-9);
        CHECK_THROW(p.swim(1.0), InvalidParameterException);
        p.finishMolt();
        p.passDay();
        CHECK_CLOSE(0.0, p.swimHoursToday(), 1e-9);
    }

    TEST(SealTricks) {
        Seal s(1, "Sammy", clownfish(), 5, 80.0, 4.0);
        CHECK(s.learnTrick("ball"));
        CHECK(!s.learnTrick("ball"));
        CHECK_THROW(s.learnTrick(""), InvalidParameterException);
        CHECK(s.perform("ball"));
        CHECK(!s.perform("hoop"));
        CHECK_EQUAL(1, s.performances());
        CHECK_CLOSE(4.8, s.dailyFoodKg(), 1e-9);
    }
}

SUITE(CoralTests) {
    TEST(GrowsInGoodWater) {
        Coral c("Acropora", 100.0);
        CHECK_CLOSE(102.0, c.grow(seaWater()), 1e-9);
    }

    TEST(BleachesInHotWater) {
        Coral c("Acropora", 100.0, 55.0);
        WaterParameters hot(30.0, 35.0, 8.2, 7.0);
        c.grow(hot);
        CHECK(c.isBleached());
        CHECK_CLOSE(100.0, c.grow(seaWater()), 1e-9);  // обесцвеченный коралл не растёт
        c.recover();
        CHECK(!c.isBleached());
    }

    TEST(Fragment) {
        Coral c("Acropora", 100.0);
        Coral frag = c.fragment(30.0);
        CHECK_CLOSE(70.0, c.size(), 1e-9);
        CHECK_CLOSE(30.0, frag.size(), 1e-9);
        CHECK_THROW(c.fragment(100.0), InvalidParameterException);
    }
}
