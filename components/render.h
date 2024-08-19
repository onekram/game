#pragma once

#include "flecs.h"
#include "init_components.h"
#include "mouse_control.h"
#include "movement.h"
#include "raylib.h"

namespace render {

struct sprite {
    std::size_t current_frame;
    std::size_t total_frames;
    std::size_t default_frame;

    float frame_swap_time;
    float elapsed_time;

    float source_width;
    float source_height;

    float dest_width;
    float dest_height;

    bool right_orientation;
};

auto render_icon_system_factory(Texture2D texture, Color tint);
auto render_system_factory(Color color);

auto render_direction_system_factory(Color color);

void sprite_system(flecs::iter& it, std::size_t, const movement::velocity& v, render::sprite& s);
void init(flecs::world& world);
} // namespace render
