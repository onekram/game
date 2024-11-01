#pragma once

#include "flecs.h"
#include "life.h"
#include "physics/physical_interaction.h"
#include "utils/init_components.h"

namespace behavior {

struct follow_tag {};

struct aiming_at_tag {};

struct player_tag {};

struct enemy_tag {};

struct explosion_tag {};

struct bullet_tag {};

struct can_damage_tag {};

struct can_restore_health_tag {};

struct can_debuff_tag {};

struct use_after_contact_tag {};

struct aid_kit_tag {};

struct tnt_barrel_tag {};

struct destroy_animation_tag {};

struct loot_box_tag {};

struct stone_tag {};

struct turret_tag {};

struct landmine_tag {};

struct landmine_prefab {};

struct zombie_prefab {};

struct weapon_debuff_tag {};

struct damage_reduction {
    float duration;
    float factor;
};

struct sound {
    std::string sound;
};

struct health_restore_points {
    float points;
};

struct get_damage {
    float points;
};

struct get_health {
    float points;
};

struct time_between_damage {
    float elapsed_time;
    float required_time;
};

struct sound_tag {};

void cause_damage_system(
    flecs::iter& it,
    std::size_t i,
    behavior::get_damage& gd,
    life::health_points& lp
);

void cause_health_restore_system(flecs::entity e, get_health& gh, life::health_points& lp);

void handle_damage_system(flecs::iter& it, std::size_t i, life::damage_points& dp);

void handle_health_restore_system(flecs::iter& it, std::size_t i, health_restore_points& hrp);

void handle_loot_box_system(flecs::iter& it, std::size_t i);

void use_after_contact_system(flecs::iter& it, std::size_t i);

void handle_sound_system(flecs::iter& it, std::size_t i, const sound& s);

void init(flecs::world& world);
} // namespace behavior
