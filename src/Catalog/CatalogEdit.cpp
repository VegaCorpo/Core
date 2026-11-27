#include "CatalogEdit.hpp"
#include <algorithm>
#include <cmath>

bool core::CatalogEdit::apply(core::BodyCatalog& catalog, const common::BodyPatch& patch)
{
    const auto body = std::ranges::find(catalog.bodies, patch.id, &core::BodyEntry::id);

    if (body == catalog.bodies.end() || !core::CatalogEdit::_isValid(patch))
        return false;

    if (patch.name)
        body->name = *patch.name;
    if (patch.mass)
        body->mass = *patch.mass;
    if (patch.radius)
        body->radius = *patch.radius;
    return true;
}

bool core::CatalogEdit::_isValid(const common::BodyPatch& patch)
{
    return (!patch.name || core::CatalogEdit::_isValidName(*patch.name)) &&
        (!patch.mass || core::CatalogEdit::_isValidMass(*patch.mass)) &&
        (!patch.radius || core::CatalogEdit::_isValidRadius(*patch.radius)) &&
        (!patch.position || core::CatalogEdit::_isFinite(patch.position->x, patch.position->y, patch.position->z)) &&
        (!patch.velocity || core::CatalogEdit::_isFinite(patch.velocity->x, patch.velocity->y, patch.velocity->z));
}

bool core::CatalogEdit::_isValidName(const common::components::Name& name)
{
    return name.value[0] != '\0' && std::ranges::find(name.value, '\0') != std::ranges::end(name.value);
}

bool core::CatalogEdit::_isValidMass(const common::components::Mass& mass)
{
    const double value = static_cast<double>(mass.mantissa) * std::pow(10.0, mass.exponent);

    return std::isfinite(value) && value > 0.0;
}

bool core::CatalogEdit::_isValidRadius(const common::components::Radius& radius)
{
    return std::isfinite(radius.value) && radius.value > 0.0F;
}

bool core::CatalogEdit::_isFinite(double x, double y, double z)
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}
