#pragma once

#include "flecs.h"
#include "life.h"
#include "movement.h"

namespace render {

struct sprite {
    std::size_t current_frame;
    std::size_t total_frames;
    std::size_t default_frame;
    std::size_t frames_per_line;

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

void sprite_velocity_system(flecs::iter& it, std::size_t, const movement::velocity& v, sprite& s);
void sprite_system(flecs::iter& it, std::size_t, sprite& s);

void angle_sprite_system(const movement::velocity& v, sprite_angle& sa);

Color health_points_color_proportional(float k);

void health_points_render_system(
    const movement::position& p,
    const life::health_points& hp,
    const sprite& s
);

void player_health_points_render_system(const life::health_points& hp);

void player_inventory_render_system(flecs::entity e);
void init(flecs::world& world);
} // namespace render
