#pragma once

#include "flecs.h"
#include "pugixml.hpp"

#include <string>

namespace entity_from_xml {
void load_levels(flecs::world& world, const std::string& path);
} // namespace entity_from_xml
