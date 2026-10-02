#pragma once

#include <components/acceleration.hpp>
#include <components/angularVelocity.hpp>
#include <components/mass.hpp>
#include <components/orientation.hpp>
#include <components/position.hpp>
#include <components/radius.hpp>
#include <components/velocity.hpp>
#include <cstddef>
#include <entt/entity/registry.hpp>
#include <types/World.hpp>

namespace core {
    class PhysicsSync {
        public:
            static void gather(const entt::registry& registry, common::SpecificDataPhysics& world);

            static std::size_t scatter(entt::registry& registry, const common::SpecificDataPhysics& world);

            [[nodiscard]] static std::size_t consistentSize(const common::SpecificDataPhysics& world) noexcept;

        private:
            static void _prepare(common::SpecificDataPhysics& world, std::size_t capacity);
            static void _gatherRotation(const entt::registry& registry, entt::entity entity,
                                        common::SpecificDataPhysics& world);
            static void _scatterOrientation(entt::registry& registry, entt::entity entity,
                                            const common::SpecificDataPhysics& world, std::size_t index);

            template <typename Component>
            [[nodiscard]] static Component _componentOrDefault(const entt::registry& registry, entt::entity entity)
            {
                const auto* component = registry.try_get<Component>(entity);
                return component != nullptr ? *component : Component{};
            }

            static auto _simulableView(const entt::registry& registry)
            {
                return registry.view<const common::components::Position, const common::components::Velocity,
                                     const common::components::Acceleration, const common::components::Mass,
                                     const common::components::Radius>();
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
