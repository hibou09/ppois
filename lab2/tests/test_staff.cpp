#include <UnitTest++/UnitTest++.h>

#include <memory>

#include "Exceptions.h"
#include "Staff.h"
#include "TestHelpers.h"

using namespace ocean;
using namespace testutil;

namespace {
Date hired() { return Date(1, 1, 2020); }
}  // namespace

SUITE(EmployeeTests) {
    TEST(BasicEmployee) {
        Employee e(1, "Ivan", 1000.0, hired());
        CHECK_EQUAL(6, e.yearsOfService(today()));
        CHECK_CLOSE(1000.0, e.monthlySalary(), 1e-9);
        e.giveRaise(10.0);
        CHECK_CLOSE(1100.0, e.monthlySalary(), 1e-9);
        CHECK_THROW(e.giveRaise(60.0), InvalidParameterException);
        e.dismiss();
        CHECK(!e.isActive());
        CHECK_THROW(Employee(1, "X", 0.0, hired()), InvalidParameterException);
    }

    TEST(PolymorphicSalaries) {
        Aquarist aq(1, "A", 1000.0, hired(), 3);
        Veterinarian vet(2, "V", 1000.0, hired(), "L1", Date(1, 1, 2030), "fish");
        Diver diver(3, "D", 1000.0, hired(), 10.0);
        Guide guide(4, "G", 1000.0, hired(), {"ru", "en", "de"});
        const Employee* staff[] = {&aq, &vet, &diver, &guide};
        double expected[] = {1200.0, 1300.0, 1200.0, 1100.0};
        for (int i = 0; i < 4; ++i) {
            CHECK_CLOSE(expected[i], staff[i]->monthlySalary(), 1e-9);
        }
        CHECK_EQUAL("Veterinarian", vet.position());
    }
}

SUITE(AquaristTests) {
    TEST(AssignTanksWithinLimit) {
        Aquarist aq(1, "A", 1000.0, hired(), 1);
        Tank a(1, "A", 5000, seaWater(), hall(), 5);
        Tank b(2, "B", 5000, seaWater(), hall(), 5);
        Tank c(3, "C", 5000, seaWater(), hall(), 5);
        aq.assignTank(&a);
        CHECK_THROW(aq.assignTank(&a), InvalidParameterException);
        aq.assignTank(&b);
        CHECK_THROW(aq.assignTank(&c), EmployeeNotQualifiedException);
        CHECK(aq.isResponsibleFor(2));
        CHECK(!aq.isResponsibleFor(3));
    }

    TEST(ReefRequiresLevel2) {
        Aquarist junior(1, "J", 1000.0, hired(), 1);
        CoralReefTank reef(1, "Reef", 5000, seaWater(), hall(), 5, 2.0);
        CHECK_THROW(junior.assignTank(&reef), EmployeeNotQualifiedException);
        CHECK(junior.promote());
        junior.assignTank(&reef);
        CHECK(junior.promote());
        CHECK(!junior.promote());
    }

    TEST(InspectAndClean) {
        Aquarist aq(1, "A", 1000.0, hired(), 2);
        Tank t(1, "T", 5000, seaWater(), hall(), 5);
        Tank foreign(2, "F", 5000, seaWater(), hall(), 5);
        aq.assignTank(&t);
        CHECK(aq.inspect(t));
        t.water().addAmmonia(0.4);
        CHECK(!aq.inspect(t));
        aq.cleanTank(t);
        CHECK_CLOSE(0.28, t.water().ammonia(), 1e-9);
        CHECK_EQUAL(2, aq.inspections());
        CHECK_THROW(aq.cleanTank(foreign), EmployeeNotQualifiedException);
    }
}

SUITE(VeterinarianTests) {
    TEST(ExamineAndTreat) {
        Veterinarian vet(1, "Dr. Aibolit", 1000.0, hired(), "L-1", Date(1, 1, 2030), "fish");
        Fish f(1, "Nemo", clownfish(), 2, 2.0, 9.0, true);
        vet.examine(f, today(), "fin rot", 4);
        CHECK(f.health().hasSeriousCondition());
        Medication m("Antibiotic", 5.0, 100.0, Date(1, 1, 2027));
        CHECK_CLOSE(10.0, vet.treat(f, m, today()), 1e-9);
        CHECK(!f.health().hasSeriousCondition());
        CHECK_EQUAL(2, static_cast<int>(f.health().entries().size()));
        CHECK_EQUAL(1, vet.examinations());
    }

    TEST(ExpiredLicense) {
        Veterinarian vet(1, "Dr", 1000.0, hired(), "L-1", Date(1, 1, 2026), "fish");
        Fish f(1, "Nemo", clownfish(), 2, 2.0, 9.0, true);
        CHECK(!vet.isLicenseValid(today()));
        CHECK_THROW(vet.examine(f, today(), "x", 1), LicenseExpiredException);
        CHECK_THROW(vet.renewLicense(Date(1, 1, 2025)), InvalidDateException);
        vet.renewLicense(Date(1, 1, 2031));
        vet.examine(f, today(), "x", 1);
        CHECK_EQUAL(1, vet.examinations());
    }
}

SUITE(DiverTrainerGuideTests) {
    TEST(DiverDepth) {
        Diver d(1, "D", 1000.0, hired(), 5.0);
        OpenOceanTank deep(1, "Deep", 1e6, seaWater(), hall(), 10, 8.0, 0.0);
        CHECK(!d.canDiveIn(deep));
        CHECK_THROW(d.dive(deep, 1.0), EmployeeNotQualifiedException);
        d.upgradeCertification(10.0);
        d.dive(deep, 2.0);
        CHECK_CLOSE(2.0, d.diveHours(), 1e-9);
        CHECK_THROW(d.dive(deep, 5.0), InvalidParameterException);
        CHECK_THROW(d.upgradeCertification(9.0), InvalidParameterException);
    }

    TEST(TrainerShow) {
        Trainer tr(1, "T", 1000.0, hired(), "pinnipeds");
        Seal a(1, "A", clownfish(), 5, 80.0, 4.0);
        Seal b(2, "B", clownfish(), 5, 80.0, 4.0);
        CHECK(tr.train(a, "ball"));
        CHECK(tr.train(a, "hoop"));
        CHECK(tr.train(b, "wave"));
        Show show("Seal show", TimeSlot(720, 780), 100);
        CHECK_THROW(tr.conductShow(show), InvalidParameterException);
        show.addPerformer(&a);
        show.addPerformer(&b);
        CHECK_EQUAL(3, tr.conductShow(show));
        CHECK_EQUAL(1, tr.showsConducted());
        show.cancel();
        CHECK_THROW(tr.conductShow(show), InvalidParameterException);
    }

    TEST(GuideLanguagesAndTours) {
        Guide g(1, "G", 1000.0, hired(), {"ru"});
        Guide other(2, "O", 1000.0, hired(), {"en"});
        CHECK(g.speaks("ru"));
        CHECK(!g.speaks("en"));
        g.learnLanguage("en");
        g.learnLanguage("en");
        CHECK_EQUAL(2, static_cast<int>(g.languages().size()));
        CHECK_THROW(Guide(3, "X", 1000.0, hired(), {}), InvalidParameterException);

        Tank t(1, "T", 5000, seaWater(), hall(), 5);
        Tour tour(1, TimeSlot(600, 660), 10);
        Visitor v(1, "V", 30, "v@a.by", 0);
        tour.addStop(&t);
        tour.addVisitor(v);
        tour.assignGuide(&g);
        CHECK_THROW(other.conductTour(tour), EmployeeNotQualifiedException);
        CHECK_EQUAL(1, g.conductTour(tour));
        CHECK(tour.isCompleted());
        CHECK_EQUAL(1, g.toursGiven());
    }

    TEST(CashierShift) {
        Cashier c(1, "C", 1000.0, hired(), 3);
        TicketOffice office(20.0, 100);
        Visitor v(1, "V", 30, "v@a.by", 100);
        c.sellTicket(office, v, TicketType::Adult, today());
        c.sellTicket(office, v, TicketType::Adult, today());
        CHECK_EQUAL(2, c.ticketsSold());
        CHECK_CLOSE(40.0, c.closeShift(), 1e-9);
        CHECK_CLOSE(0.0, c.shiftRevenue(), 1e-9);
        c.dismiss();
        CHECK_THROW(c.sellTicket(office, v, TicketType::Adult, today()),
                    EmployeeNotQualifiedException);
    }
}

SUITE(ScheduleTests) {
    TEST(ShiftOverlap) {
        Shift a(1, today(), TimeSlot(480, 960));
        CHECK(a.overlaps(Shift(1, today(), TimeSlot(900, 1000))));
        CHECK(!a.overlaps(Shift(2, today(), TimeSlot(900, 1000))));
        CHECK(!a.overlaps(Shift(1, today().addDays(1), TimeSlot(900, 1000))));
        CHECK_CLOSE(8.0, a.hours(), 1e-9);
    }

    TEST(StaffScheduleOperations) {
        StaffSchedule s;
        s.addShift(Shift(1, today(), TimeSlot(480, 960)));
        s.addShift(Shift(1, today().addDays(1), TimeSlot(480, 720)));
        s.addShift(Shift(2, today(), TimeSlot(600, 1200)));
        CHECK_THROW(s.addShift(Shift(1, today(), TimeSlot(900, 1000))),
                    ScheduleConflictException);
        CHECK_CLOSE(12.0, s.totalHours(1), 1e-9);
        CHECK_EQUAL(2, static_cast<int>(s.shiftsOn(today()).size()));
        CHECK(s.isWorking(2, today(), 700));
        CHECK(!s.isWorking(2, today(), 500));
        CHECK_EQUAL(2, s.removeShiftsOf(1));
        CHECK_CLOSE(0.0, s.totalHours(1), 1e-9);
    }
}
