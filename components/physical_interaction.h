#pragma once
#include "flecs.h"
#include "init_components.h"
#include "movement.h"

namespace physical_interaction {

struct repulsion_radius {
    float radius;
};

struct interaction_radius {
    float radius;
};

struct interaction_tag {};

void repulsion_system(
    flecs::iter& it,
    std::size_t i,
    movement::position& p,
    const repulsion_radius& rr
);

void interaction_handle_system(
    flecs::iter& it,
    std::size_t i,
    const movement::position& p1,
    const interaction_radius& ir1
);

void init(flecs::world& world);
} // namespace physical_interaction
