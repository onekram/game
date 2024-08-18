#pragma once

#include "flecs.h"
#include "init_components.h"

namespace life {
struct life_time {
    float time;
};

struct life_points {
    float points;
};

struct damage_points {
    float points;
};

void life_time_system(flecs::iter& it, std::size_t, life_time& lt);
void life_points_system(flecs::entity& e, life_points);
void init(flecs::world& world);
}; // namespace life
