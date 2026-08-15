#pragma once

#include <components/acceleration.hpp>
#include <components/mass.hpp>
#include <components/position.hpp>
#include <components/velocity.hpp>
#include <cstddef>
#include <entt/entity/registry.hpp>
#include <types/World.hpp>

namespace core {
    class PhysicsSync {
        public:
            static void gather(const entt::registry& registry, common::WorldState& world);

            static std::size_t scatter(entt::registry& registry, const common::WorldState& world);

            [[nodiscard]] static std::size_t consistentSize(const common::WorldState& world) noexcept;

        private:
            static auto _simulableView(const entt::registry& registry)
            {
                return registry.view<const common::components::Position, const common::components::Velocity,
                                     const common::components::Acceleration, const common::components::Mass>();
            }

            [[nodiscard]] static std::size_t _toIdentifier(entt::entity entity) noexcept
            {
                return static_cast<std::size_t>(entt::to_integral(entity));
            }
            [[nodiscard]] static entt::entity _toEntity(std::size_t identifier) noexcept
            {
                return static_cast<entt::entity>(static_cast<entt::id_type>(identifier));
            }
    };
} // namespace core
