#pragma once

#include <entt/entity/registry.hpp>
#include <exception>
#include <mutex>

namespace core {
    enum class SimulationState
    {
        OK,
        INITIALIZATION_ERROR,
        SIMULATION_ERROR,
    };

    class Simulation {
        public:
            core::SimulationState initializeCore() noexcept;
            void launchSimulation();

        private:
            void _launchPhysics();
            void _launchRenderer();
            SimulationState _loadEngines() noexcept;

            entt::registry _registry;

            double physicsThreshold = 1;
            double physicsAccumulator = 0.0;
            std::mutex physicsMutex;

            double rendererThreshold = 0.016;
            double rendererAccumulator = 0.0;
            std::mutex rendererMutex;

            bool is_running = true;
    };
} // namespace core
