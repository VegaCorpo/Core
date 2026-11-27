#pragma once

#include <components/angularVelocity.hpp>
#include <components/mass.hpp>
#include <components/name.hpp>
#include <components/radius.hpp>
#include <components/texture.hpp>
#include <cstddef>
#include <vector>

namespace core {
    struct BodyEntry {
            std::size_t id;
            common::components::Name name;
            common::components::Mass mass;
            common::components::Radius radius;
            common::components::Texture texture;
            common::components::AngularVelocity angularVelocity;
    };

    struct BodyCatalog {
            std::vector<core::BodyEntry> bodies;
            std::size_t nextId = 0;
    };
} // namespace core
