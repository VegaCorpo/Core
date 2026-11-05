#include "RenderSync.hpp"
#include <cstddef>

void core::RenderSync::gather(const common::SceneDescription& scene, common::SpecificDataRender& data)
{
    const std::size_t count = scene.bodies.size();

    data.entitiesId.clear();
    data.names.clear();
    data.radius.clear();
    data.textures.clear();

    data.entitiesId.reserve(count);
    data.names.reserve(count);
    data.radius.reserve(count);
    data.textures.reserve(count);

    for (const common::BodyDescription& body : scene.bodies) {
        data.entitiesId.push_back(body.id);
        data.names.push_back(body.name);
        data.radius.push_back(body.radius);
        data.textures.push_back(body.texture);
    }
}
