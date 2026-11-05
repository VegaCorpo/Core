#pragma once

#include <types/Scene.hpp>
#include <types/World.hpp>

namespace core {
    class UISync {
        public:
            static void gather(const common::SceneDescription& scene, common::SpecificDataUI& data);
    };
} // namespace core
