#pragma once

#include <components/name.hpp>
#include <components/radius.hpp>
#include <components/texture.hpp>
#include <cstddef>
#include <entt/entity/registry.hpp>
#include <types/World.hpp>

namespace core {
    class RenderSync {
        public:
            static void gather(const entt::registry& registry, common::SpecificDataRender& data);

            [[nodiscard]] static std::size_t consistentSize(const common::SpecificDataRender& data) noexcept;

        private:
            static auto _renderableView(const entt::registry& registry)
            {
                return registry.view<const common::components::Name, const common::components::Radius,
                                     const common::components::Texture>();
            }

            [[nodiscard]] static std::size_t _toIdentifier(entt::entity entity) noexcept
            {
                return static_cast<std::size_t>(entt::to_integral(entity));
            }
    };
} // namespace core
