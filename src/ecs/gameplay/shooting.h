#pragma once

#include "flecs.h"
#include "inventory/container.h"
#include "movement.h"

namespace shooting {
struct shoot_direction {
    float src_x;
    float src_y;
    float dst_x;
    float dst_y;
};

struct check_tag {};

struct firing_range {
    float value;
};

struct time_between_shots {
    float elapsed_time;
    float shot_time;
};

void handle_shoot_system(
    flecs::entity player,
    const movement::position& p,
    const mouse_control::mouse& mc
);

void bullet_spawn_system(
    flecs::iter& it,
    std::size_t i,
    const shoot_direction& sd,
    const container::attack_factor* k
);

void range_system(flecs::iter& it, std::size_t i, firing_range& fr, const movement::velocity&);

void reloading_system(flecs::entity container);

void time_between_shots_system(
    flecs::iter& it,
    std::size_t i,
    shooting::time_between_shots& tbs,
    const shoot_direction& sd
);

void shots_system(flecs::iter& it, std::size_t i, const shoot_direction& sd);

void aiming_at_player_system(flecs::entity e, const movement::position& p);

void init(flecs::world& world);
} // namespace shooting
