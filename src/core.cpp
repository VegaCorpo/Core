#include <chrono>
#include <components/acceleration.hpp>
#include <components/mass.hpp>
#include <components/position.hpp>
#include <components/velocity.hpp>
#include <mutex>
#include <thread>
#include "SharedLoader/SharedLoader.hpp"

#include <iostream>
#include "core.hpp"

constexpr float EARTH_MANTISSA = 5.97;
constexpr int EARTH_EXPONENT = 24;
constexpr float MOON_MANTISSA = 7.35;
constexpr int MOON_EXPONENT = 22;
constexpr float SUN_MANTISSA = 1.98;
constexpr int SUN_EXPONENT = 30;

core::SimulationState core::Simulation::initializeCore() noexcept
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

    // auto state = this->_loadEngines();
    // if (state != core::SimulationState::OK) {
    //     return state;
    // }
    using EngineFactory = std::function<std::unique_ptr<std::any>()>;

    utils::SharedLoader loader;
    loader.load<std::function<std::unique_ptr<std::any>()>>("plugins/Renderer/liborbital_render", "renderer");
    auto renderApi = loader.get<EngineFactory>("renderer");
    auto renderEngine = renderApi();
    return core::SimulationState::OK;
}

core::SimulationState core::Simulation::_loadEngines() noexcept
{
    // if (this->_loadPhysics() != core::SimulationState::OK) {
    //     return core::SimulationState::INITIALIZATION_ERROR;
    // }
    // if (this->_loadRenderer() != core::SimulationState::OK) {
    //     return core::SimulationState::INITIALIZATION_ERROR;
    // }
    // if (this->_loadUI() != core::SimulationState::OK) {
    //     return core::SimulationState::INITIALIZATION_ERROR;
    // }
    // if (this->_loadInputs() != core::SimulationState::OK) {
    //     return core::SimulationState::INITIALIZATION_ERROR;
    // }
    // if (this->_loadLoader() != core::SimulationState::OK) {
    //     return core::SimulationState::INITIALIZATION_ERROR;
    // }

    return core::SimulationState::OK;
}

void core::Simulation::launchSimulation()
{
    std::thread physicsThread(&core::Simulation::_launchPhysics, this);
    std::thread rendererThread(&core::Simulation::_launchRenderer, this);

    physicsThread.detach();
    rendererThread.detach();

    auto prev = std::chrono::high_resolution_clock::now();
    while (this->is_running) {
        auto time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> deltaTime = time - prev;
        prev = time;
        {
            std::scoped_lock lock(this->physicsMutex);
            this->physicsAccumulator += deltaTime.count();
        }
        {
            std::scoped_lock lock(this->rendererMutex);
            this->rendererAccumulator += deltaTime.count();
        }
    }
}

void core::Simulation::_launchPhysics()
{
    while (this->is_running) {
        {
            std::scoped_lock lock(this->physicsMutex);
            if (this->physicsAccumulator >= this->physicsThreshold) {
                // Update cpy transform data
                this->physicsAccumulator = 0;
            }
        }
    }
}

void core::Simulation::_launchRenderer()
{
    while (this->is_running) {
        {
            std::scoped_lock lock(this->rendererMutex);
            if (this->rendererAccumulator >= this->rendererThreshold) {
                // render operation
                this->rendererAccumulator = 0;
            }
        }
    }
}
