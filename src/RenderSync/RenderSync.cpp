#include "RenderSync.hpp"
#include <algorithm>
#include <entt/entity/entity.hpp>

void core::RenderSync::gather(const entt::registry& registry, common::SpecificDataRender& data)
{
    const auto view = core::RenderSync::_renderableView(registry);
    const std::size_t hint = view.size_hint();

    data.entitiesId.clear();
    data.names.clear();
    data.radius.clear();
    data.textures.clear();

    data.entitiesId.reserve(hint);
    data.names.reserve(hint);
    data.radius.reserve(hint);
    data.textures.reserve(hint);

    for (const auto [entity, name, radius, texture] : view.each()) {
        data.entitiesId.push_back(core::RenderSync::_toIdentifier(entity));
        data.names.push_back(name);
        data.radius.push_back(radius);
        data.textures.push_back(texture);
    }
}

std::size_t core::RenderSync::consistentSize(const common::SpecificDataRender& data) noexcept
{
    return std::min({data.entitiesId.size(), data.names.size(), data.radius.size(), data.textures.size()});
}
