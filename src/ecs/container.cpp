#include "container.h"

#include "behavior.h"
#include "life.h"
#include "render.h"
#include "shooting.h"
#include "string_modify.h"
#include "textures.h"

#include <iostream>

flecs::entity container::item_kind(flecs::entity item) {
    flecs::world world = item.world();
    flecs::entity result;

    item.each([&](flecs::id id) {
        if (id.is_entity()) {
            if (id.entity().has(flecs::IsA, world.id<item_tag>())) {
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

flecs::entity container::item_type(flecs::entity item) {
    flecs::world world = item.world();
    flecs::entity result;

    item.each([&](flecs::id id) {
        if (id.is_entity()) {
            if (id.entity().has(flecs::IsA, world.id<item_tag>())) {
                result = id.entity();
            }
        } else if (id.is_pair()) {
            if (id.first() == flecs::IsA) {
                flecs::entity base_kind = item_kind(id.second());
                if (base_kind) {
                    result = id.second();
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
            if (id.entity().has(flecs::IsA, world.id<item_tag>())) {
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

    return modify(result, "_tag");
}

flecs::entity container::get_container(flecs::entity container) {
    if (container.has<container_tag>()) {
        return container;
    }
    return container.target<inventory_tag>();
}

flecs::entity
container::find_item_w_kind(flecs::entity container, flecs::entity kind, bool active_required) {
    flecs::entity result;
    container = get_container(container);
    for_each_item(container, [&](flecs::entity item) {
        if (active_required) {
            if (!item.has<active_tag>()) {
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

flecs::entity container::find_item_w_kind(
    flecs::entity container,
    flecs::entity kind,
    flecs::entity type,
    bool active_required
) {
    flecs::entity result;
    container = get_container(container);
    for_each_item(container, [&](flecs::entity item) {
        if (active_required) {
            if (!item.has<active_tag>()) {
                return;
            }
        }

        flecs::entity ik = item_kind(item);
        flecs::entity it = item_type(item);
        if (ik == kind && it == type) {
            result = item;
        }
    });

    return result;
}

flecs::entity container::find_item_active(flecs::entity container) {
    flecs::entity result;
    container = get_container(container);
    for_each_item(container, [&](flecs::entity item) {
        if (item.has<active_tag>()) {
            result = item;
        }
    });

    return result;
}

void container::transfer_item(flecs::entity container, flecs::entity item, std::int32_t max_count) {
    quantity* amt = item.get_mut<quantity>();
    if (amt) {
        flecs::entity ik = item_kind(item);
        flecs::entity it = item_type(item);
        flecs::entity dst_item = find_item_w_kind(container, ik, it);
        if (dst_item) {
            quantity& dst_amt = dst_item.ensure<quantity>();
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
        item.add<contained_by_tag>(container);
    }
}

void container::transfer_items(flecs::entity dst, flecs::entity src) {
    dst.world().defer([&] {
        dst = get_container(dst);
        src = get_container(src);
        for_each_item(src, [&](flecs::entity item) { transfer_item(dst, item); });
    });
}

void container::print_items(flecs::entity container, std::size_t shift) {
    container = get_container(container);

    int32_t count = 0;
    for_each_item(container, [&](flecs::entity item) {
        int32_t amount = 1;
        item.get([&](const quantity& a) { amount = a.value; });

        std::cout << std::string(shift, ' ') << " - " << amount << " " << item_name(item);
        if (amount > 1) {
            std::cout << "s";
        }

        int32_t cartridges;
        int32_t max;
        if (item.get([&cartridges, &max](const magazine& m, const magazine_size& ms) {
                cartridges = m.value;
                max = ms.value;
            })) {
            std::cout << ' ' << cartridges << "/" << max;
        }

        std::cout << " (" << item_kind(item).name() << ")\n";

        if (item.has<container_tag>()) {
            print_items(item, shift + 2);
        }
        count++;
    });

    if (!count) {
        std::cout << " - << empty >>\n";
    }
}

std::int32_t container::count_items(flecs::entity container, bool hold) {
    container = get_container(container);
    int32_t count = 0;
    for_each_item(container, [&hold, &count](flecs::entity item) {
        if (hold) {
            if (!item.has<can_hold_tag>()) {
                return;
            }
        }
        int32_t amount = 1;
        item.get([&](const quantity& a) { amount = a.value; });
        count += amount;
    });
    return count;
}

void container::reloading_weapons(flecs::entity container) {
    flecs::world world = container.world();
    container = get_container(container);

    flecs::entity active_weapon =
        find_item_w_kind(container, world.entity<ranged_weapon_tag>(), true);
    if (active_weapon) {
        flecs::entity cartridges = find_item_w_kind(
            container,
            world.entity<ammo_tag>(),
            active_weapon.target<load_with_tag>()
        );
        if (cartridges) {
            int32_t max;
            if (active_weapon.get([&max](const magazine_size& ms) { max = ms.value; })) {
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
            if (item.has<can_hold_tag>()) {
                item.set<number>({index});
                ++index;
            }
        });
    });
}

void container::choose_active(flecs::entity container) {
    container = get_container(container);
    auto active = find_item_active(container);
    if (active) {
        return;
    }

    bool selected = false;

    for_each_item(container, [&](flecs::entity item) {
        if (selected) {
            return;
        }
        item.add<active_tag>();
        selected = true;
    });
}

void container::set_active(flecs::entity container, std::ptrdiff_t i) {
    container = get_container(container);
    auto active = find_item_active(container);
    if (!active) {
        return;
    }

    const number* num = active.get<number>();
    if (!num) {
        return;
    }
    active.remove<active_tag>();

    std::int32_t count = count_items(container, true);

    std::size_t index = (num->value + count + (i % count)) % count;

    container.world().defer([&] {
        for_each_item(container, [&index](flecs::entity item) {
            const number* n = item.get<number>();
            if (n && n->value == index) {
                item.add<active_tag>();
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
        if (item.has<ammo_tag>()) {
            res = item;
        }
    });

    return res;
}

void container::init(flecs::world& world) {
    world.system<mouse_control::mouse>("MouseActiveItemSystem")
        .kind(flecs::PostUpdate)
        .with<inventory_tag>(flecs::Wildcard)
        .each(mouse_active_inventory_item);

    world.system<>("ChooseActive")
        .kind(flecs::OnStart)
        .with<inventory_tag>(flecs::Wildcard)
        .each(choose_active);

    world.system<>("NumberInventorySystem")
        .kind(flecs::OnStart)
        .with<inventory_tag>(flecs::Wildcard)
        .each(number_container_elements);

    world.system<>("PrintContainerSystem")
        .interval(4)
        .with<inventory_tag>(flecs::Wildcard)
        .each([](flecs::entity e) {
            std::cout << '\n';
            print_items(e);
        })
        .disable();
}
