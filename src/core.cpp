#include "core.hpp"
#include <components/mass.hpp>

void core::Simulation::initializeCore()
{
    this->initializeCoreEntity();
    this->initializeCoreSystem();
}

void core::Simulation::initializeCoreEntity()
{
    for (auto i = 0u; i < 10u; ++i) {
        const auto entity = this->_registry.create();
        this->_registry.emplace<components::Mass>(entity);
    }
}

void core::Simulation::initializeCoreSystem() {}
