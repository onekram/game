#pragma once

#include "flecs.h"
#include "life.h"
#include "movement.h"

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
    Texture2D texture;
};

struct sprite_angle {
    float angle;
};

auto render_icon_system_factory(Color tint);
auto render_system_factory(Color color);
auto render_direction_system_factory(Color color);

void sprite_system(flecs::iter& it, std::size_t, const movement::velocity& v, render::sprite& s);

void angle_sprite_system(const movement::velocity& v, sprite_angle& sa);

void health_points_render_system(
    const movement::position& p,
    const life::health_points& lp,
    const sprite& s
);

void player_health_points_render_system(const life::health_points& hp);

void init(flecs::world& world);
} // namespace render
