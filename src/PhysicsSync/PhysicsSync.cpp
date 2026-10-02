#include "PhysicsSync.hpp"
#include <algorithm>
#include <entt/entity/entity.hpp>

void core::PhysicsSync::gather(const entt::registry& registry, common::SpecificDataPhysics& world)
{
    const auto view = core::PhysicsSync::_simulableView(registry);

    core::PhysicsSync::_prepare(world, view.size_hint());

    for (const auto [entity, position, velocity, acceleration, mass, radius] : view.each()) {
        world.entitiesId.push_back(core::PhysicsSync::_toIdentifier(entity));
        world.positions.push_back(position);
        world.velocities.push_back(velocity);
        world.accelerations.push_back(acceleration);
        world.masses.push_back(mass);
        world.radius.push_back(radius);
        core::PhysicsSync::_gatherRotation(registry, entity, world);
    }
}

std::size_t core::PhysicsSync::scatter(entt::registry& registry, const common::SpecificDataPhysics& world)
{
    const std::size_t count = core::PhysicsSync::consistentSize(world);
    std::size_t updated = 0;

    for (std::size_t i = 0; i < count; i += 1) {
        const entt::entity entity = core::PhysicsSync::_toEntity(world.entitiesId[i]);

        if (!registry.valid(entity))
            continue;

        auto [position, velocity, acceleration] =
            registry
                .try_get<common::components::Position, common::components::Velocity, common::components::Acceleration>(
                    entity);

        if (position == nullptr || velocity == nullptr || acceleration == nullptr)
            continue;

        *position = world.positions[i];
        *velocity = world.velocities[i];
        *acceleration = world.accelerations[i];
        core::PhysicsSync::_scatterOrientation(registry, entity, world, i);
        updated += 1;
    }
    return updated;
}

std::size_t core::PhysicsSync::consistentSize(const common::SpecificDataPhysics& world) noexcept
{
    return std::min({world.entitiesId.size(), world.positions.size(), world.velocities.size(),
                     world.accelerations.size(), world.masses.size()});
}

void core::PhysicsSync::_prepare(common::SpecificDataPhysics& world, std::size_t capacity)
{
    world.entitiesId.clear();
    world.positions.clear();
    world.velocities.clear();
    world.accelerations.clear();
    world.masses.clear();
    world.radius.clear();
    world.orientations.clear();
    world.angularVelocities.clear();

    world.entitiesId.reserve(capacity);
    world.positions.reserve(capacity);
    world.velocities.reserve(capacity);
    world.accelerations.reserve(capacity);
    world.masses.reserve(capacity);
    world.radius.reserve(capacity);
    world.orientations.reserve(capacity);
    world.angularVelocities.reserve(capacity);
}

void core::PhysicsSync::_gatherRotation(const entt::registry& registry, entt::entity entity,
                                        common::SpecificDataPhysics& world)
{
    world.orientations.push_back(core::PhysicsSync::_componentOrDefault<common::components::Orientation>(registry, entity));
    world.angularVelocities.push_back(core::PhysicsSync::_componentOrDefault<common::components::AngularVelocity>(registry, entity));
}

void core::PhysicsSync::_scatterOrientation(entt::registry& registry, entt::entity entity,
                                            const common::SpecificDataPhysics& world, std::size_t index)
{
    auto* orientation = registry.try_get<common::components::Orientation>(entity);

    if (orientation == nullptr || index >= world.orientations.size())
        return;
    *orientation = world.orientations[index];
}
