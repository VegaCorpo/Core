#include "UISync.hpp"
#include <cstddef>

void core::UISync::gather(const core::BodyCatalog& catalog, common::SpecificDataUI& data)
{
    const std::size_t count = catalog.bodies.size();

    data.entitiesId.clear();
    data.names.clear();
    data.masses.clear();

    data.entitiesId.reserve(count);
    data.names.reserve(count);
    data.masses.reserve(count);

    for (const core::BodyEntry& body : catalog.bodies) {
        data.entitiesId.push_back(body.id);
        data.names.push_back(body.name);
        data.masses.push_back(body.mass);
    }
}
