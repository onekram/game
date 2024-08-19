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

struct can_restore_health_tag {};

struct aid_kit_tag {};

struct health_restore_points {
    float points;
};

struct get_damage {
    float points;
};

struct get_health {
    float points;
};

struct temporary_tag {};

void cause_damage_system(flecs::entity e, get_damage& gd, life::health_points& lp);
void cause_health_restore_system(flecs::entity e, get_health& gh, life::health_points& lp);

void handle_damage_system(flecs::entity e, life::damage_points& dp);
void handle_health_restore_system(flecs::entity e, health_restore_points& hrp);

void init(flecs::world& world);
}; // namespace behavior
