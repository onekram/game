#pragma once

#include "flecs.h"
#include "pugixml.hpp"

#include <string>

namespace entity_from_xml {
void init_player(pugi::xml_node& node, flecs::world& world);

void init_enemy(pugi::xml_node& node, flecs::world& world);

void init_aid_kit(pugi::xml_node& node, flecs::world& world);

void init_tnt_barrel(pugi::xml_node& node, flecs::world& world);

void init_loot_box(pugi::xml_node& node, flecs::world& world);

void init_landmine(pugi::xml_node& node, flecs::world& world);

void init_static_turret(pugi::xml_node& node, flecs::world& world);

void init_dynamic_turret(pugi::xml_node& node, flecs::world& world);

void load_levels(flecs::world& world);
} // namespace entity_from_xml
