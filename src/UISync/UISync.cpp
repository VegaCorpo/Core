#include "UISync.hpp"
#include "src/UISync/UISync.hpp"
#include <algorithm>
#include <entt/entity/entity.hpp>

void core::UISync::gather(const entt::registry& registry, common::SpecificDataUI& data)
{
    const auto view = core::UISync::_renderableView(registry);
    const std::size_t hint = view.size_hint();

    data.entitiesId.clear();
    data.names.clear();
    data.masses.clear();

    data.entitiesId.reserve(hint);
    data.names.reserve(hint);
    data.masses.reserve(hint);

    for (const auto [entity, name, masse] : view.each()) {
        data.entitiesId.push_back(core::UISync::_toIdentifier(entity));
        data.names.push_back(name);
        data.masses.push_back(masse);
    }
}

std::size_t core::UISync::consistentSize(const common::SpecificDataUI& data) noexcept
{
    return std::min({data.entitiesId.size(), data.names.size(), data.masses.size()});
}
