#pragma once
#include "flecs.h"
#include "movement.h"

namespace render {
auto render_system_factory(Color color);
void init(flecs::world& world);
} // namespace render
