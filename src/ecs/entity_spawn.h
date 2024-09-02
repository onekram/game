#pragma once
#include "flecs.h"

#include <cstdint>

namespace entity_spawn {
auto enemy_spawn_system_factory(std::size_t count);
void player_spawn_system(flecs::iter& it);
void aid_kid_spawn_system(flecs::iter& it);
void tnt_barrel_spawn_system(flecs::iter& it);
void loot_box_spawn_system(flecs::iter& it);
void static_turret_spawn_system(flecs::iter& it);
void dynamic_turret_spawn_system(flecs::iter& it);
void landmine_spawn_system(flecs::iter& it);
void init(flecs::world& world);
} // namespace entity_spawn
