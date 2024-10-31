#pragma once

#include "flecs.h"
#include "pugixml.hpp"

#include <string>

class level {
  public:
    level(pugi::xml_node& level, flecs::world& world);
    void init();

  private:
    pugi::xml_node _level;
    flecs::world _world;

    void add_pistol(flecs::entity& ctainer);
    void add_automatic_weapon(flecs::entity& ctainer);
    void add_minigun(flecs::entity& ctainer);
    void add_enemy_weapon(flecs::entity& ctainer);
    void add_pistol_ammo(flecs::entity& ctainer, pugi::xml_node& node);
    void add_small_caliber_ammo(flecs::entity& ctainer, pugi::xml_node& node);
    void add_enemy_ammo(flecs::entity& ctainer, pugi::xml_node& node);
    void add_container(pugi::xml_node& node, flecs::entity& container);
    void add_inventory(pugi::xml_node& node, flecs::entity& entity);
    void add_coords(flecs::entity& entity, pugi::xml_node node);
    void init_player(pugi::xml_node& node);
    void init_aid_kit(pugi::xml_node& node);
    void init_loot_box(pugi::xml_node& node);
    void init_tnt_barrel(pugi::xml_node& node);
    void init_enemy(pugi::xml_node& node);
    void init_static_turret(pugi::xml_node& node);
    void init_dynamic_turret(pugi::xml_node& node);
    void init_landmine(pugi::xml_node& node);
    void init_stone(pugi::xml_node& node);
};
