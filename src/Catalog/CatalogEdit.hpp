#pragma once

#include <components/mass.hpp>
#include <components/name.hpp>
#include <components/radius.hpp>
#include <types/BodyPatch.hpp>
#include "src/Catalog/BodyCatalog.hpp"

namespace core {
    class CatalogEdit {
        public:
            static bool apply(core::BodyCatalog& catalog, const common::BodyPatch& patch);

        private:
            static bool _isValid(const common::BodyPatch& patch);
            static bool _isValidName(const common::components::Name& name);
            static bool _isValidMass(const common::components::Mass& mass);
            static bool _isValidRadius(const common::components::Radius& radius);
            static bool _isFinite(double x, double y, double z);
    };
} // namespace core
