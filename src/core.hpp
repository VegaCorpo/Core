#pragma once

#include <entt/entity/registry.hpp>


namespace core {
    class Simulation {
        public:
            void initializeCore();
        private:
            entt::registry _registry;
    };
} // namespace core
