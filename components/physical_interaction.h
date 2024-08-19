#pragma once
#include "flecs.h"
#include "init_components.h"
#include "movement.h"

namespace physical_interaction {

struct physical_interaction_tag {};

struct interaction_tag {};

void repulsion_system(flecs::iter& it, std::size_t i, movement::position& p);

void interaction_handle_system(flecs::iter& it, std::size_t i, movement::position& p1);

void log_interaction(flecs::iter& it, std::size_t i);
void init(flecs::world& world);
} // namespace physical_interaction
