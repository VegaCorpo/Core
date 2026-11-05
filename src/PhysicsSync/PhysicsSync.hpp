#pragma once

#include <cstddef>
#include <types/Scene.hpp>
#include <types/World.hpp>

namespace core {
    class PhysicsSync {
        public:
            static void gather(const common::SceneDescription& scene, common::SpecificDataPhysics& world);

        private:
            static void _prepare(common::SpecificDataPhysics& world, std::size_t capacity);
    };
} // namespace core
