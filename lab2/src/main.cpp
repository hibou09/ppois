/**
 * @file main.cpp
 * @brief Демонстрация работы модели «Океанариум».
 */
#include <iostream>
#include <memory>

#include "Exceptions.h"
#include "Management.h"

using namespace ocean;

int main() {
    const Date today(11, 11, 2026);
    Oceanarium oceanarium("Blue Planet", "Minsk", 1'000'000.0, 25.0);

    WaterRequirements tropical(23.0, 28.0, 30.0, 37.0, 7.8, 8.5, 5.0);
    Species clownfish("Clownfish", "Amphiprion ocellaris", Diet::Omnivore, tropical, false,
                      ConservationStatus::LeastConcern);
    Species blacktip("Blacktip reef shark", "Carcharhinus melanopterus", Diet::Carnivore,
                     tropical, true, ConservationStatus::Vulnerable);
    Species greenTurtle("Green sea turtle", "Chelonia mydas", Diet::Herbivore, tropical, false,
                        ConservationStatus::Endangered);

    WaterParameters seaWater(25.0, 35.0, 8.2, 7.0);
    Location hall("Tropics", "Main building", 1);

    auto& reef = oceanarium.addTank(
        std::make_unique<CoralReefTank>(1, "Coral Garden", 50'000.0, seaWater, hall, 30, 4.0));
    auto& ocean = oceanarium.addTank(std::make_unique<OpenOceanTank>(
        2, "Open Ocean", 2'000'000.0, seaWater, hall, 20, 8.0, 40.0));

    reef.addAnimal(std::make_shared<Fish>(1, "Nemo", clownfish, 2, 0.25, 9.0, true));
    reef.addAnimal(std::make_shared<Turtle>(2, "Tortilla", greenTurtle, 30, 120.0, 90.0));
    ocean.addAnimal(std::make_shared<Shark>(3, "Bruce", blacktip, 10, 25.0, 160.0, 1300.0));

    // Акулу нельзя подселить к рыбе-клоуну.
    try {
        reef.addAnimal(std::make_shared<Shark>(4, "Jaws", blacktip, 5, 20.0, 140.0, 1100.0));
    } catch (const IncompatibleSpeciesException& e) {
        std::cout << "Expected error: " << e.what() << '\n';
    }

    oceanarium.foodStorage().addItem(FoodItem("Krill", 900.0, 100.0, 12.0, Date(1, 2, 2027)));
    oceanarium.feeding().addEntry(FeedingEntry(1, "Krill", 0.01, 9 * 60));
    std::cout << "Animals fed at 09:00: " << oceanarium.feedAnimals(9 * 60, today) << '\n';

    Visitor anna(1, "Anna", 30, "anna@example.com", 100.0);
    Ticket ticket = oceanarium.ticketOffice().sell(anna, TicketType::Adult, today);
    ticket.use(today);
    std::cout << "Ticket #" << ticket.id() << " sold for " << ticket.price() << '\n';

    int tasks = oceanarium.runDailyOperations(today);
    std::cout << "Maintenance tasks created: " << tasks << '\n';
    std::cout << oceanarium.summary() << '\n';
    std::cout << "Endangered animals: " << oceanarium.endangeredAnimals() << '\n';
    return 0;
}
