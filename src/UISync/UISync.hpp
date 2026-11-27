#pragma once

#include <types/World.hpp>
#include "src/Catalog/BodyCatalog.hpp"

namespace core {
    class UISync {
        public:
            static void gather(const core::BodyCatalog& catalog, common::SpecificDataUI& data);
    };
} // namespace core
