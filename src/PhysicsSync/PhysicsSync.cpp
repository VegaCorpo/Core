#include "PhysicsSync.hpp"
#include <algorithm>
#include <unordered_map>

void core::PhysicsSync::gather(const common::SceneDescription& scene, common::SpecificDataPhysics& world)
{
    core::PhysicsSync::_prepare(world, scene.bodies.size());

    for (const common::BodyDescription& body : scene.bodies) {
        world.entitiesId.push_back(body.id);
        world.positions.push_back(body.position);
        world.velocities.push_back(body.velocity);
        world.accelerations.push_back(body.acceleration);
        world.masses.push_back(body.mass);
        world.radius.push_back(body.radius);
        world.orientations.push_back(body.orientation);
        world.angularVelocities.push_back(body.angularVelocity);
    }
    world.integrator = scene.integrator;
}

void core::PhysicsSync::absorb(const common::WorldState& state, common::SpecificDataPhysics& world)
{
    const std::size_t count = std::min(
        {state.entitiesId.size(), state.positions.size(), state.velocities.size(), state.accelerations.size()});
    std::unordered_map<std::size_t, std::size_t> indexOf;

    indexOf.reserve(world.entitiesId.size());
    for (std::size_t i = 0; i < world.entitiesId.size(); i += 1)
        indexOf.emplace(world.entitiesId[i], i);

    for (std::size_t i = 0; i < count; i += 1) {
        const auto found = indexOf.find(state.entitiesId[i]);

        if (found == indexOf.end())
            continue;

        const std::size_t index = found->second;

        world.positions[index] = state.positions[i];
        world.velocities[index] = state.velocities[i];
        world.accelerations[index] = state.accelerations[i];
        if (i < state.orientations.size())
            world.orientations[index] = state.orientations[i];
    }
}

void core::PhysicsSync::apply(const common::BodyPatch& patch, common::SpecificDataPhysics& world)
{
    const auto found = std::ranges::find(world.entitiesId, patch.id);

    if (found == world.entitiesId.end())
        return;

    const auto index = static_cast<std::size_t>(found - world.entitiesId.begin());

    if (patch.mass)
        world.masses[index] = *patch.mass;
    if (patch.radius)
        world.radius[index] = *patch.radius;
    if (patch.position)
        world.positions[index] = *patch.position;
    if (patch.velocity)
        world.velocities[index] = *patch.velocity;
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
