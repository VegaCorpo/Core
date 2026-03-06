#include <chrono>
#include <components/acceleration.hpp>
#include <components/mass.hpp>
#include <components/position.hpp>
#include <components/velocity.hpp>
#include <entt/entity/fwd.hpp>
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

    this->_loadEngines();
    return core::SimulationState::OK;
}

core::SimulationState core::Simulation::_loadEngines() noexcept
{

    this->_loader.load<void(void*, double)>("plugins/Physics/liborbital_physics", "physicsUpdate", "physicsUpdate");
    this->_loader.load<std::string()>("plugins/Physics/liborbital_physics", "getName", "getName");

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
                this->physicsAccumulator = 0;
                auto getName = this->_loader.get<std::string()>("getName");
                std::cout << getName() << std::endl;
                auto updatePhysics = this->_loader.get<void(void*, double)>("physicsUpdate");
                updatePhysics(&this->_registry, 3.4);
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
