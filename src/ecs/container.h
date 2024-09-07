#pragma once
#include "flecs.h"
#include "mouse_control.h"

#include <limits>
#include <string>
#include <vector>

namespace container {
struct item_tag {};

struct container_tag {};

struct inventory_tag {};

struct contained_by_tag {};

struct active_tag {};

struct attack_factor {
    float k;
};

struct quantity {
    std::int32_t value;
};

struct magazine_size {
    std::int32_t value;
};

struct magazine {
    std::int32_t value;
};

struct Number {
    std::size_t value;
};

struct RangedWeapon {};

struct ammo_tag {};

struct armor_tag {};

struct automatic_weapon_tag {};

struct pistol_tag {};

struct minigun_tag {};

struct can_hold_tag {};

struct automatic_tag {};

struct small_caliber_ammo_tag {};

struct pistol_ammo_tag {};

struct enemy_ammo_tag {};

struct enemy_turret_tag {};

struct load_with_tag {};

flecs::entity item_kind(flecs::entity item);
flecs::entity item_type(flecs::entity item);

std::string item_name(flecs::entity item);

flecs::entity get_container(flecs::entity container);

template <typename Func>
inline void for_each_item(flecs::entity container, const Func& func) {
    container.world().query_builder().with<contained_by_tag>(container).each(func);
}

flecs::entity
find_item_w_kind(flecs::entity container, flecs::entity kind, bool active_required = false);

flecs::entity find_item_w_kind(
    flecs::entity container,
    flecs::entity kind,
    flecs::entity type,
    bool active_required = false
);

flecs::entity find_item_active(flecs::entity container);

void transfer_item(
    flecs::entity container,
    flecs::entity item,
    std::int32_t max_count = std::numeric_limits<std::int32_t>::max()
);

void transfer_items(flecs::entity dst, flecs::entity src);

void attack(flecs::entity player, flecs::entity weapon);

void print_items(flecs::entity container, std::size_t shift = 0);

std::int32_t count_items(flecs::entity container, bool hold = false);

void reloading_weapons(flecs::entity container);

void number_container_elements(flecs::entity container);

void set_active(flecs::entity container, std::ptrdiff_t i);

flecs::entity get_cartridges_from_weapon(flecs::entity weapon);

void mouse_active_inventory_item(flecs::entity container, mouse_control::mouse& m);

void init(flecs::world& world);
} // namespace container
