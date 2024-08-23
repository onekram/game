#include "container.h"

#include "behavior.h"
#include "life.h"
#include "render.h"
#include "shooting.h"
#include "textures.h"

#include <iostream>

flecs::entity container::item_kind(flecs::entity item) {
    flecs::world world = item.world();
    flecs::entity result;

    item.each([&](flecs::id id) {
        if (id.is_entity()) {
            if (id.entity().has(flecs::IsA, world.id<Item>())) {
                result = id.entity();
            }
        } else if (id.is_pair()) {
            if (id.first() == flecs::IsA) {
                flecs::entity base_kind = item_kind(id.second());
                if (base_kind) {
                    result = base_kind;
                }
            }
        }
    });

    return result;
}

std::string container::item_name(flecs::entity item) {
    flecs::world world = item.world();
    std::string result;

    item.each([&](flecs::id id) {
        if (id.is_entity()) {
            if (id.entity().has(flecs::IsA, world.id<Item>())) {
                result = id.entity().name();
            }
        } else if (id.is_pair()) {
            if (id.first() == flecs::IsA) {
                flecs::entity base_kind = item_kind(id.second());
                if (base_kind) {
                    result = id.second().name();
                }
            }
        }
    });

    return result;
}

flecs::entity container::get_container(flecs::entity container) {
    if (container.has<Container>()) {
        return container;
    }
    return container.target<Inventory>();
}

flecs::entity
container::find_item_w_kind(flecs::entity container, flecs::entity kind, bool active_required) {
    flecs::entity result;
    container = get_container(container);
    for_each_item(container, [&](flecs::entity item) {
        if (active_required) {
            if (!item.has<Active>()) {
                return;
            }
        }

        flecs::entity ik = item_kind(item);
        if (ik == kind) {
            result = item;
        }
    });

    return result;
}

flecs::entity container::find_item_active(flecs::entity container) {
    flecs::entity result;
    container = get_container(container);
    for_each_item(container, [&](flecs::entity item) {
        if (item.has<Active>()) {
            result = item;
        }
    });

    return result;
}

void container::transfer_item(flecs::entity container, flecs::entity item, std::int32_t max_count) {
    Amount* amt = item.get_mut<Amount>();
    if (amt) {
        flecs::world ecs = container.world();
        flecs::entity ik = item_kind(item);
        flecs::entity dst_item = find_item_w_kind(container, ik);
        if (dst_item) {
            Amount& dst_amt = dst_item.ensure<Amount>();
            if (max_count >= amt->value) {
                dst_amt.value += amt->value;
                item.destruct();
            } else {
                dst_amt.value += max_count;
                amt->value -= max_count;
            }
            return;
        }
    }

    if (max_count > 1) {
        item.add<ContainedBy>(container);
    }
}

void container::transfer_items(flecs::entity dst, flecs::entity src) {
    std::cout << ">> Transfer items from " << src.name() << " to " << dst.name() << "\n\n";

    // Defer, because we're adding/removing components while we're iterating
    dst.world().defer([&] {
        dst = get_container(dst);
        src = get_container(src);

        for_each_item(src, [&](flecs::entity item) { transfer_item(dst, item); });
    });
}

void container::attack(flecs::entity player, flecs::entity weapon) {
    flecs::world ecs = player.world();

    std::cout << ">> " << player.name() << " is attacked with a " << item_name(weapon) << "!\n";

    const Attack* att = weapon.get<Attack>();
    if (!att) {
        // A weapon without Attack power? Odd.
        std::cout << " - the weapon is a dud\n";
        return;
    }

    int att_value = att->value;

    // Get armor item, if player has equipped any
    flecs::entity armor = find_item_w_kind(player, ecs.entity<Armor>(), true);
    if (armor) {
        Health* armor_health = armor.get_mut<Health>();
        if (!armor_health) {
            // Armor without Defense power? Odd.
            std::cout << " - the " << item_name(armor) << " armor is a dud\n";
        } else {
            std::cout << " - " << player.name() << " defends with " << item_name(armor) << "\n";

            // Subtract attack from armor health. If armor health goes below
            // zero, delete the armor and carry over remaining attack points.
            armor_health->value -= att_value;
            if (armor_health->value <= 0) {
                att_value += armor_health->value;
                armor.destruct();
                std::cout << " - " << item_name(armor) << " is destroyed!\n";
            } else {
                std::cout << " - " << item_name(armor) << " has " << armor_health->value
                          << " health left after taking " << att_value << " damage\n";
                att_value = 0;
            }
        }
    } else {
        // Brave but stupid
        std::cout << " - " << player.name() << " fights without armor!\n";
    }

    // For each usage of the weapon, subtract one from its health
    Health& weapon_health = weapon.ensure<Health>();
    if (!--weapon_health.value) {
        std::cout << " - " << item_name(weapon) << " is destroyed!\n";
        weapon.destruct();
    } else {
        std::cout << " - " << item_name(weapon) << " has " << weapon_health.value << " uses left";
    }

    // If armor didn't counter the whole attack, subtract from the player health
    if (att_value) {
        Health& player_health = player.ensure<Health>();
        if (!(player_health.value -= att_value)) {
            std::cout << " - " << player.name() << " died!\n";
            player.destruct();
        } else {
            std::cout << " - " << player.name() << " has " << player_health.value
                      << " health left after taking " << att_value << " damage\n";
        }
    }

    std::cout << "\n";
}

// Print items in inventory
void container::print_items(flecs::entity container, std::size_t shift) {
    container = get_container(container);

    int32_t count = 0;
    for_each_item(container, [&](flecs::entity item) {
        int32_t amount = 1;
        item.get([&](const Amount& a) { amount = a.value; });

        std::cout << std::string(shift, ' ') << " - " << amount << " " << item_name(item);
        if (amount > 1) {
            std::cout << "s";
        }

        int32_t cartridges;
        int32_t max;
        if (item.get([&cartridges, &max](const Magazine& m, const MagazineSize& ms) {
                cartridges = m.value;
                max = ms.value;
            })) {
            std::cout << ' ' << cartridges << "/" << max;
        }

        std::cout << " (" << item_kind(item).name() << ")\n";

        if (item.has<Container>()) {
            print_items(item, shift + 2);
        }
        count++;
    });

    if (!count) {
        std::cout << " - << empty >>\n";
    }
}

std::int32_t container::count_items(flecs::entity container, bool hold) {
    int32_t count = 0;
    for_each_item(container, [&hold, &count](flecs::entity item) {
        if (hold) {
            if (!item.has<CanHold>()) {
                return;
            }
        }
        int32_t amount = 1;
        item.get([&](const Amount& a) { amount = a.value; });
        count += amount;
    });
    return count;
}

void container::reloading_weapons(flecs::entity container) {
    flecs::world world = container.world();
    container = get_container(container);

    flecs::entity active_weapon = find_item_w_kind(container, world.entity<RangedWeapon>(), true);
    if (active_weapon) {
        flecs::entity cartridges = find_item_w_kind(container, world.entity<Cartridge>());
        if (cartridges) {
            int32_t max;
            if (active_weapon.get([&max](const MagazineSize& ms) { max = ms.value; })) {
                int32_t count = count_items(active_weapon);
                if (count < max) {
                    transfer_item(active_weapon, cartridges, max - count);
                }
            } else {
                transfer_item(active_weapon, cartridges, 1);
            }

        } else {
            std::cout << "No cartridges for this weapon\n";
        }
    } else {
        std::cout << "No active weapon chosen\n";
    }
}

void container::number_container_elements(flecs::entity container) {
    container = get_container(container);

    std::size_t index = 0;
    container.world().defer([&] {
        for_each_item(container, [&index](flecs::entity item) {
            if (item.has<CanHold>()) {
                item.set<Number>({index});
                ++index;
            }
        });
    });
}

void container::set_active(flecs::entity container, std::ptrdiff_t i) {
    container = get_container(container);
    auto active = find_item_active(container);
    if (!active) {
        return;
    }

    const Number* num = active.get<Number>();
    if (!num) {
        return;
    }
    active.remove<Active>();

    std::int32_t count = count_items(container, true);

    std::size_t index = (num->value + count + (i % count)) % count;

    container.world().defer([&] {
        for_each_item(container, [&index](flecs::entity item) {
            const Number* n = item.get<Number>();
            if (n && n->value == index) {
                item.add<Active>();
            }
        });
    });
}

void container::mouse_active_inventory_item(flecs::entity container, mouse_control::mouse& m) {
    container = get_container(container);
    if (m.scroll_amount > 0) {
        set_active(container, 1);
    } else if (m.scroll_amount < 0) {
        set_active(container, -1);
    }
}

flecs::entity container::get_cartridges_from_weapon(flecs::entity weapon) {
    flecs::entity res;
    for_each_item(weapon, [&res](flecs::entity item) {
        if (item.has<Cartridge>()) {
            res = item;
        }
    });

    return res;
}

void container::init(flecs::world& world) {
    world.component<ContainedBy>().add(flecs::Exclusive);

    world.component<RangedWeapon>().is_a<Item>();
    world.component<Cartridge>().is_a<Item>();

    world.prefab<AutomaticWeapon>()
        .add<RangedWeapon>()
        .add<CanHold>()
        .add<Automatic>()
        .set<MagazineSize>({30});

    world.prefab<Gun>().add<RangedWeapon>().add<CanHold>().set<MagazineSize>({10});

    world.prefab<JustCartridge>()
        .add<Cartridge>()
        .set<life::damage_points>({20})
        .add<behavior::bullet_tag>()
        .add<behavior::can_damage_tag, behavior::enemy_tag>()
        .add<behavior::can_damage_tag, behavior::tnt_barrel_tag>()
        .add<life::temporary_tag>()
        .set_auto_override<shooting::firing_range>({6})
        .add<render::sprite_angle>()
        .set<physical_interaction::interaction_radius>({3});

    world.system<mouse_control::mouse>("MouseActiveItemSystem")
        .kind(flecs::PostUpdate)
        .with<Inventory>(flecs::Wildcard)
        .each(mouse_active_inventory_item);

    world.system<>("NumerInventorySystem")
        .kind(flecs::OnStart)
        .with<Inventory>(flecs::Wildcard)
        .each(number_container_elements);

    world.system<>("PrintContainerSystem")
        .interval(4)
        .with<Inventory>(flecs::Wildcard)
        .each([](flecs::entity e) {
            std::cout << '\n';
            print_items(e);
        })
        .disable();
}
