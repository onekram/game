#pragma once
#include "flecs.h"
#include "init_components.h"
#include "life.h"
#include "physical_interaction.h"

namespace behavior {
struct follow_tag {};

struct player_tag {};

struct enemy_tag {};

struct bullet_tag {};

struct can_damage_tag {};

struct get_damage {
    float points;
};

void cause_damage_system(flecs::entity e, get_damage& gd, life::life_points& lp);

void handle_damage_system(flecs::entity e, life::damage_points& dp);

void init(flecs::world& world);
}; // namespace behavior
