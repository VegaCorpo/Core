#include "PhysicsSync.hpp"

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
