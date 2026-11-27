#include "CatalogSync.hpp"
#include <algorithm>

void core::CatalogSync::gather(const common::SceneDescription& scene, core::BodyCatalog& catalog)
{
    catalog.bodies.clear();
    catalog.bodies.reserve(scene.bodies.size());
    catalog.nextId = 0;

    for (const common::BodyDescription& body : scene.bodies) {
        catalog.bodies.push_back({.id = body.id,
                                  .name = body.name,
                                  .mass = body.mass,
                                  .radius = body.radius,
                                  .texture = body.texture,
                                  .angularVelocity = body.angularVelocity});
        catalog.nextId = std::max(catalog.nextId, body.id + 1);
    }
}
