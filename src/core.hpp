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

            double physicsAccumulator = 0.0;
            std::mutex physicsMutex;

            double rendererAccumulator = 0.0;
            std::mutex rendererMutex;
            
            bool is_running = true;
    };
} // namespace core
