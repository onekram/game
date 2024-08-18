#pragma once

#include "flecs.h"
#include "init_components.h"

namespace life_time {
struct life_time {
    float time;
};

void life_time_system(flecs::iter& it, std::size_t, life_time& lt);
void init(flecs::world& world);
}; // namespace life_time
