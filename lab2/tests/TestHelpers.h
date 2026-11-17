/**
 * @file TestHelpers.h
 * @brief Общие фабрики тестовых объектов.
 */
#pragma once

#include "Animals.h"
#include "Common.h"
#include "Water.h"

namespace testutil {

inline ocean::WaterParameters seaWater() {
    return ocean::WaterParameters(25.0, 35.0, 8.2, 7.0, 0.0);
}

inline ocean::WaterParameters coldWater() {
    return ocean::WaterParameters(6.0, 33.0, 8.0, 8.0, 0.0);
}

inline ocean::WaterRequirements tropical() {
    return ocean::WaterRequirements(23.0, 28.0, 30.0, 37.0, 7.8, 8.5, 5.0);
}

inline ocean::WaterRequirements cold() {
    return ocean::WaterRequirements(2.0, 12.0, 28.0, 36.0, 7.5, 8.5, 6.0);
}

inline ocean::Species clownfish() {
    return ocean::Species("Clownfish", "Amphiprion ocellaris", ocean::Diet::Omnivore, tropical(),
                          false, ocean::ConservationStatus::LeastConcern);
}

inline ocean::Species shark() {
    return ocean::Species("Blacktip", "Carcharhinus melanopterus", ocean::Diet::Carnivore,
                          tropical(), true, ocean::ConservationStatus::Vulnerable);
}

inline ocean::Species turtle() {
    return ocean::Species("Green turtle", "Chelonia mydas", ocean::Diet::Herbivore, tropical(),
                          false, ocean::ConservationStatus::Endangered);
}

inline ocean::Species penguin() {
    return ocean::Species("Gentoo", "Pygoscelis papua", ocean::Diet::Carnivore, cold(), false,
                          ocean::ConservationStatus::LeastConcern);
}

inline ocean::Location hall() {
    return ocean::Location("Tropics", "Main", 1);
}

inline ocean::Date today() {
    return ocean::Date(11, 11, 2026);
}

}  // namespace testutil
