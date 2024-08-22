#pragma once
#include "flecs.h"

#include <limits>
#include <string>

namespace container {
struct Item {};

struct Container {};

struct Inventory {};

struct ContainedBy {};

struct Active {};

struct Amount {
    std::int32_t value;
};

struct Health {
    std::int32_t value;
};

struct MagazineSize {
    std::int32_t value;
};

struct Magazine {
    std::int32_t value;
};

struct Attack {
    std::int32_t value;
};

struct RangedWeapon {};

struct Cartridge {};

struct Armor {};

struct AutomaticWeapon {};

struct Gun {};

struct WoodenArmor {};

struct IronArmor {};

flecs::entity item_kind(flecs::entity item);

std::string item_name(flecs::entity item);

flecs::entity get_container(flecs::entity container);

template <typename Func>
void for_each_item(flecs::entity container, const Func& func);

flecs::entity
find_item_w_kind(flecs::entity container, flecs::entity kind, bool active_required = false);

void transfer_item(
    flecs::entity container,
    flecs::entity item,
    std::int32_t max_count = std::numeric_limits<std::int32_t>::max()
);

void transfer_items(flecs::entity dst, flecs::entity src);

void attack(flecs::entity player, flecs::entity weapon);

void print_items(flecs::entity container, std::size_t shift = 0);

std::int32_t count_items(flecs::entity container);

void reloading_weapons(flecs::entity container);

void init(flecs::world& world);
} // namespace container
