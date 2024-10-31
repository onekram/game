#include "level.h"

#include "behavior/behavior.h"
#include "gameplay/movement.h"
#include "inventory/container.h"

void level::add_pistol(flecs::entity& ctainer) {
    _world.entity()
        .is_a<container::pistol_tag>()
        .add<container::container_tag>()
        .with<container::contained_by_tag>([&] {
            _world.entity().is_a<container::pistol_ammo_tag>().set<container::quantity>({10});
        })
        .add<container::contained_by_tag>(ctainer);
}

void level::add_automatic_weapon(flecs::entity& ctainer) {
    _world.entity()
        .is_a<container::automatic_weapon_tag>()
        .add<container::container_tag>()
        .with<container::contained_by_tag>([&] {
            _world.entity().is_a<container::small_caliber_ammo_tag>().set<container::quantity>({30}
            );
        })
        .add<container::contained_by_tag>(ctainer);
}

void level::add_minigun(flecs::entity& ctainer) {
    _world.entity()
        .is_a<container::minigun_tag>()
        .add<container::container_tag>()
        .with<container::contained_by_tag>([&] {
            _world.entity().is_a<container::small_caliber_ammo_tag>().set<container::quantity>({1000
            });
        })
        .add<container::contained_by_tag>(ctainer);
}

void level::add_enemy_weapon(flecs::entity& ctainer) {
    _world.entity()
        .is_a<container::enemy_turret_tag>()
        .add<container::container_tag>()
        .add<container::active_tag>()
        .with<container::contained_by_tag>([&] {
            _world.entity().is_a<container::enemy_ammo_tag>().set<container::quantity>({100});
        })
        .add<container::contained_by_tag>(ctainer);
}

void level::add_pistol_ammo(flecs::entity& ctainer, pugi::xml_node& node) {
    _world.entity()
        .is_a<container::pistol_ammo_tag>()
        .set<container::quantity>({node.attribute("Amount").as_int()})
        .add<container::contained_by_tag>(ctainer);
}

void level::add_small_caliber_ammo(flecs::entity& ctainer, pugi::xml_node& node) {
    _world.entity()
        .is_a<container::small_caliber_ammo_tag>()
        .set<container::quantity>({node.attribute("Amount").as_int()})
        .add<container::contained_by_tag>(ctainer);
}

void level::add_enemy_ammo(flecs::entity& ctainer, pugi::xml_node& node) {
    _world.entity()
        .is_a<container::enemy_ammo_tag>()
        .set<container::quantity>({node.attribute("Amount").as_int()})
        .add<container::contained_by_tag>(ctainer);
}

void level::add_container(pugi::xml_node& node, flecs::entity& container) {
    container.add<container::container_tag>();
    for (pugi::xml_node inner_node : node.children()) {
        if (std::string(inner_node.name()) == "Pistol") {
            add_pistol(container);
        } else if (std::string(inner_node.name()) == "AutomaticWeapon") {
            add_automatic_weapon(container);
        } else if (std::string(inner_node.name()) == "Minigun") {
            add_minigun(container);
        } else if (std::string(inner_node.name()) == "EnemyWeapon") {
            add_enemy_weapon(container);
        } else if (std::string(inner_node.name()) == "SmallCaliberAmmo") {
            add_small_caliber_ammo(container, inner_node);
        } else if (std::string(inner_node.name()) == "PistolAmmo") {
            add_pistol_ammo(container, inner_node);
        } else if (std::string(inner_node.name()) == "EnemyAmmo") {
            add_enemy_ammo(container, inner_node);
        }
    }
}

void level::add_inventory(pugi::xml_node& node, flecs::entity& entity) {
    flecs::entity container = _world.entity().add<container::container_tag>();
    entity.add<container::inventory_tag>(container);

    add_container(node, container);
}

void level::add_coords(flecs::entity& entity, pugi::xml_node node) {
    float x = node.attribute("X").as_float();
    float y = node.attribute("Y").as_float();
    entity.set<movement::position>({x, y});
}

void level::init_player(pugi::xml_node& node) {
    flecs::entity player = _world.entity().is_a<behavior::player_tag>();
    add_coords(player, node);
    add_inventory(node, player);
}

void level::init_aid_kit(pugi::xml_node& node) {
    flecs::entity aid_kit = _world.entity().is_a<behavior::aid_kit_tag>();
    add_coords(aid_kit, node);
}

void level::init_loot_box(pugi::xml_node& node) {
    flecs::entity loot_box = _world.entity().is_a<behavior::loot_box_tag>();
    add_coords(loot_box, node);
    add_container(node, loot_box);
}

void level::init_tnt_barrel(pugi::xml_node& node) {
    flecs::entity tnt_barrel = _world.entity().is_a<behavior::tnt_barrel_tag>();
    add_coords(tnt_barrel, node);
}

void level::init_enemy(pugi::xml_node& node) {
    flecs::entity enemy = _world.entity().is_a<behavior::zombie_prefab>();
    add_coords(enemy, node);
}

void level::init_static_turret(pugi::xml_node& node) {
    flecs::entity turret = _world.entity().is_a<behavior::turret_tag>();
    add_coords(turret, node);

    add_inventory(node, turret);
}

void level::level::init_dynamic_turret(pugi::xml_node& node) {
    flecs::entity turret = _world.entity().is_a<behavior::turret_tag>();
    float x1 = node.attribute("X1").as_float();
    float y1 = node.attribute("Y1").as_float();
    turret.set<movement::position>({x1, y1});
    turret.set<movement::position, movement::begin_tag>({x1, y1});

    float x2 = node.attribute("X2").as_float();
    float y2 = node.attribute("Y2").as_float();
    turret.set<movement::position, movement::end_tag>({x2, y2});

    add_inventory(node, turret);
}

void level::init_landmine(pugi::xml_node& node) {
    flecs::entity landmine = _world.entity().is_a<behavior::landmine_prefab>();
    add_coords(landmine, node);
}

void level::init_stone(pugi::xml_node& node) {
    flecs::entity stone = _world.entity().is_a<behavior::stone_tag>();
    add_coords(stone, node);
}

void level::init_debuff(pugi::xml_node& node) {
    flecs::entity debuff = _world.entity().is_a<behavior::weapon_debuff_tag>();
    add_coords(debuff, node);
    debuff.set<behavior::damage_reduction>({node.attribute("Duration").as_float(), node.attribute("Factor").as_float()});
}

void level::init() {
    for (pugi::xml_node entity : _level.children()) {
        if (std::string(entity.name()) == "Player") {
            init_player(entity);
        } else if (std::string(entity.name()) == "AidKit") {
            init_aid_kit(entity);
        } else if (std::string(entity.name()) == "Barrel") {
            init_tnt_barrel(entity);
        } else if (std::string(entity.name()) == "Enemy") {
            init_enemy(entity);
        } else if (std::string(entity.name()) == "StaticTurret") {
            init_static_turret(entity);
        } else if (std::string(entity.name()) == "DynamicTurret") {
            init_dynamic_turret(entity);
        } else if (std::string(entity.name()) == "LandMine") {
            init_landmine(entity);
        } else if (std::string(entity.name()) == "LootBox") {
            init_loot_box(entity);
        } else if (std::string(entity.name()) == "Stone") {
            init_stone(entity);
        } else if (std::string(entity.name()) == "WeaponDebuff") {
            init_debuff(entity);
        }
    }
}

level::level(pugi::xml_node& level, flecs::world& _world)
    : _level(level)
    , _world(_world) {}
