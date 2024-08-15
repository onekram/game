#pragma once

#include "flecs.h"
#include "init_components.h"
#include "mouse_control.h"
#include "movement.h"
#include "raylib.h"

namespace render {
struct icon_type {
    bool stand;
    bool right;
};

auto render_icon_system_factory(float scale, Color tint);

auto render_direction_system_factory(Color tint);

void init(flecs::world& world);
} // namespace render
