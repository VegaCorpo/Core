#include "UISync.hpp"
#include <cstddef>

void core::UISync::gather(const common::SceneDescription& scene, common::SpecificDataUI& data)
{
    const std::size_t count = scene.bodies.size();

    data.entitiesId.clear();
    data.names.clear();
    data.masses.clear();

    data.entitiesId.reserve(count);
    data.names.reserve(count);
    data.masses.reserve(count);

    for (const common::BodyDescription& body : scene.bodies) {
        data.entitiesId.push_back(body.id);
        data.names.push_back(body.name);
        data.masses.push_back(body.mass);
    }
}
