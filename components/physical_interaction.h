#pragma once
#include "flecs.h"
#include "init_components.h"
#include "movement.h"

namespace physical_interaction {

using collision_query_type = flecs::query<movement::position>;

struct physical_interaction_tag {};

void repulsion_system(flecs::iter& it, std::size_t i, movement::position& p);

void init(flecs::world& world);
} // namespace physical_interaction
