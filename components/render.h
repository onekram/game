#pragma once

#include "flecs.h"
#include "functional"
#include "init_components.h"
#include "movement.h"
#include "raylib.h"

namespace render {
struct icon_type {
    bool stand;
    bool right;
};

auto render_icon_system_factory(const std::vector<Texture2D>& textures, float scale, Color tint);
void init(flecs::world& world);
} // namespace render
