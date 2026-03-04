#include <components/acceleration.hpp>
#include <components/mass.hpp>
#include <components/position.hpp>
#include <components/velocity.hpp>

#include "core.hpp"

constexpr float EARTH_MANTISSA = 5.97;
constexpr int EARTH_EXPONENT = 24;
constexpr float MOON_MANTISSA = 7.35;
constexpr int MOON_EXPONENT = 22;
constexpr float SUN_MANTISSA = 1.98;
constexpr int SUN_EXPONENT = 30;

void core::Simulation::initializeCore()
{
    const auto earthEntity = this->_registry.create();
    this->_registry.emplace<components::Mass>(earthEntity, components::Mass(EARTH_MANTISSA, EARTH_EXPONENT));
    this->_registry.emplace<components::Acceleration>(earthEntity, components::Acceleration(0, 0));
    this->_registry.emplace<components::Position>(earthEntity, components::Position(149597870.f, 0.f, 0.f));
    this->_registry.emplace<components::Velocity>(earthEntity, components::Velocity(0.f, 0.f, 0.f));

    const auto moonEntity = this->_registry.create();
    this->_registry.emplace<components::Mass>(moonEntity, components::Mass(MOON_MANTISSA, MOON_EXPONENT));
    this->_registry.emplace<components::Acceleration>(moonEntity, components::Acceleration(0, 0));
    this->_registry.emplace<components::Position>(moonEntity, components::Position(149213470.f, 0.f, 0.f));
    this->_registry.emplace<components::Velocity>(moonEntity, components::Velocity(0.f, 0.f, 0.f));

    const auto sunEntity = this->_registry.create();
    this->_registry.emplace<components::Mass>(sunEntity, components::Mass(SUN_MANTISSA, SUN_EXPONENT));
    this->_registry.emplace<components::Acceleration>(sunEntity, components::Acceleration(0, 0));
    this->_registry.emplace<components::Position>(sunEntity, components::Position(0.0f, 0.f, 0.f));
    this->_registry.emplace<components::Velocity>(sunEntity, components::Velocity(0.f, 0.f, 0.f));
}
