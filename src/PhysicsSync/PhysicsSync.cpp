#include "PhysicsSync.hpp"
#include <algorithm>
#include <entt/entity/entity.hpp>

void core::PhysicsSync::gather(const entt::registry& registry, common::WorldState& world)
{
    const auto view = core::PhysicsSync::_simulableView(registry);
    const std::size_t hint = view.size_hint();

    world.entities.clear();
    world.positions.clear();
    world.velocities.clear();
    world.accelerations.clear();
    world.mass.clear();

    world.entities.reserve(hint);
    world.positions.reserve(hint);
    world.velocities.reserve(hint);
    world.accelerations.reserve(hint);
    world.mass.reserve(hint);

    for (const auto [entity, position, velocity, acceleration, mass] : view.each()) {
        world.entities.push_back(core::PhysicsSync::_toIdentifier(entity));
        world.positions.push_back(position);
        world.velocities.push_back(velocity);
        world.accelerations.push_back(acceleration);
        world.mass.push_back(mass);
    }
}

std::size_t core::PhysicsSync::scatter(entt::registry& registry, const common::WorldState& world)
{
    const std::size_t count = core::PhysicsSync::consistentSize(world);
    std::size_t updated = 0;

    for (std::size_t i = 0; i < count; i += 1) {
        const entt::entity entity = core::PhysicsSync::_toEntity(world.entities[i]);

        if (!registry.valid(entity))
            continue;

        auto [position, velocity, acceleration] =
            registry.try_get<common::components::Position, common::components::Velocity,
                             common::components::Acceleration>(entity);

        if (position == nullptr || velocity == nullptr || acceleration == nullptr)
            continue;

        *position = world.positions[i];
        *velocity = world.velocities[i];
        *acceleration = world.accelerations[i];
        updated += 1;
    }
    return updated;
}

std::size_t core::PhysicsSync::consistentSize(const common::WorldState& world) noexcept
{
    return std::min({world.entities.size(), world.positions.size(), world.velocities.size(),
                     world.accelerations.size(), world.mass.size()});
}
