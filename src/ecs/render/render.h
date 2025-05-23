#pragma once

#include "behavior/life.h"
#include "flecs.h"
#include "gameplay/movement.h"
#include "inventory/container.h"

namespace render {

struct sprite {
    std::size_t current_frame;
    std::size_t default_frame;
    std::size_t total_frames;
    std::size_t frames_per_line;

    float source_width;
    float source_height;

    float dest_width;
    float dest_height;

    bool right_orientation;
    std::string texture;
};

struct sprite_swap {
    float frame_swap_time;
    float elapsed_time;
};

struct sprite_angle {
    float angle;
};

struct rotation {};

void render_icon_system_factory(
    const movement::position& p,
    const sprite& s,
    const sprite_angle* sa
);

auto render_system_factory(Color color);

auto render_direction_system_factory(Color color);

void sprite_system(flecs::iter& it, std::size_t, sprite& s, sprite_swap& ss);

void angle_sprite_system(const movement::velocity& v, sprite_angle& sa);

Color health_points_color_proportional(float k);

void health_points_render_system(
    const movement::position& p,
    const life::health_points& hp,
    const sprite* s
);

void player_health_points_render_system(const life::health_points& hp);

void player_inventory_render_system(flecs::entity a);

void stage_ammo_render_system(flecs::entity player);

void sprite_velocity_system(
    flecs::iter& it,
    std::size_t,
    const movement::velocity& v,
    sprite& st,
    sprite_swap& s
);

void sprite_rotation_at_target_system(flecs::entity e, const movement::position& p, sprite& s);

void init(flecs::world& world);
} // namespace render
