#pragma once

#include <types/Scene.hpp>
#include <types/World.hpp>

namespace core {
    class RenderSync {
        public:
            static void gather(const common::SceneDescription& scene, common::SpecificDataRender& data);
    };
} // namespace core
