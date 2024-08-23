#pragma once
#include "flecs.h"
#include "movement.h"

namespace shooting {
struct Shot {};

struct ShotFrom {};

void spawn_bullet(
    flecs::iter& it,
    std::size_t,
    const movement::position& p,
    const mouse_control::mouse& m
);

struct firing_range {
    float value;
};

struct time_between_shots {
    float elapsed_time;
    float shot_time;
};

void handle_shoot_system(flecs::entity player, mouse_control::mouse& mc);

void bullet_spawn_system(flecs::iter& it, std::size_t i);

void range_system(flecs::iter& it, std::size_t i, firing_range& fr, const movement::velocity&);

void reloading_system(flecs::entity container);

void time_between_shots_system(flecs::iter& it, std::size_t i, shooting::time_between_shots& tbs);
void shots_system(flecs::iter& it, std::size_t i);

void init(flecs::world& world);
} // namespace shooting
