#pragma once

#include <types/Scene.hpp>
#include "src/Catalog/BodyCatalog.hpp"

namespace core {
    class CatalogSync {
        public:
            static void gather(const common::SceneDescription& scene, core::BodyCatalog& catalog);
    };
} // namespace core
