#pragma once

#include <entt/entity/registry.hpp>

namespace core {
    class Simulation {
        public:
            void initializeCore();
            void launchSimulation();
        private:
            void _launchPhysics();
            void _launchRenderer();

            entt::registry _registry;
            double accumulator = 0;
            
            bool is_running = true;
    };
} // namespace core
