#pragma once

#include "flecs.h"
#include "init_components.h"
#include "movement.h"

namespace life {
struct life_time {
    float time;
};

struct health_points {
    float max;
    float points;
};

struct damage_points {
    float points;
};

struct temporary_tag {};

struct destroy_tag {};

struct already_done_tag {};

void life_time_system(flecs::iter& it, std::size_t, life_time& lt);
void health_points_system(flecs::entity e, health_points& lp);
void destroy_entity_system(flecs::entity e);
void destroy_action_explosion(flecs::entity e, const movement::position& p);

void init(flecs::world& world);
} // namespace life
