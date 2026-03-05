#pragma once

#include <entt/entity/registry.hpp>
#include <mutex>

namespace core {
    class Simulation {
        public:
            void initializeCore();
            void launchSimulation();
        private:
            void _launchPhysics();
            void _launchRenderer();

            entt::registry _registry;
            double accumulatorPhysics = 0.0;
            std::mutex physicsMutex;
            
            bool is_running = true;
    };
} // namespace core
