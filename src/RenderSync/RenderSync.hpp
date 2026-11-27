#pragma once

#include <types/World.hpp>
#include "src/Catalog/BodyCatalog.hpp"

namespace core {
    class RenderSync {
        public:
            static void gather(const core::BodyCatalog& catalog, common::SpecificDataRender& data);
    };
} // namespace core
