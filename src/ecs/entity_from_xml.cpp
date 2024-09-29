#include "entity_from_xml.h"

#include "behavior.h"
#include "container.h"

#include <iostream>

void add_pistol(flecs::world& world, flecs::entity& ctainer) {
    world.entity()
        .is_a<container::pistol_tag>()
        .add<container::container_tag>()
        .with<container::contained_by_tag>([&] {
            world.entity().is_a<container::pistol_ammo_tag>().set<container::quantity>({10});
        })
        .add<container::contained_by_tag>(ctainer);
}

void add_automatic_weapon(flecs::world& world, flecs::entity& ctainer) {
    world.entity()
        .is_a<container::automatic_weapon_tag>()
        .add<container::container_tag>()
        .with<container::contained_by_tag>([&] {
            world.entity().is_a<container::small_caliber_ammo_tag>().set<container::quantity>({30});
        })
        .add<container::contained_by_tag>(ctainer);
}

void add_minigun(flecs::world& world, flecs::entity& ctainer) {
    world.entity()
        .is_a<container::minigun_tag>()
        .add<container::container_tag>()
        .with<container::contained_by_tag>([&] {
            world.entity().is_a<container::small_caliber_ammo_tag>().set<container::quantity>({1000}
            );
        })
        .add<container::contained_by_tag>(ctainer);
}

void add_enemy_weapon(flecs::world& world, flecs::entity& ctainer) {
    world.entity()
        .is_a<container::enemy_turret_tag>()
        .add<container::container_tag>()
        .add<container::active_tag>()
        .with<container::contained_by_tag>([&] {
            world.entity().is_a<container::enemy_ammo_tag>().set<container::quantity>({100});
        })
        .add<container::contained_by_tag>(ctainer);
}

void add_pistol_ammo(flecs::world& world, flecs::entity& ctainer, pugi::xml_node& node) {
    world.entity()
        .is_a<container::pistol_ammo_tag>()
        .set<container::quantity>({node.attribute("Amount").as_int()})
        .add<container::contained_by_tag>(ctainer);
}

void add_small_caliber_ammo(flecs::world& world, flecs::entity& ctainer, pugi::xml_node& node) {
    world.entity()
        .is_a<container::small_caliber_ammo_tag>()
        .set<container::quantity>({node.attribute("Amount").as_int()})
        .add<container::contained_by_tag>(ctainer);
}

void add_enemy_ammo(flecs::world& world, flecs::entity& ctainer, pugi::xml_node& node) {
    world.entity()
        .is_a<container::enemy_ammo_tag>()
        .set<container::quantity>({node.attribute("Amount").as_int()})
        .add<container::contained_by_tag>(ctainer);
}

void add_container(flecs::world& world, pugi::xml_node& node, flecs::entity& container) {
    container.add<container::container_tag>();
    for (pugi::xml_node inner_node : node.children()) {
        if (std::string(inner_node.name()) == "Pistol") {
            add_pistol(world, container);
        } else if (std::string(inner_node.name()) == "AutomaticWeapon") {
            add_automatic_weapon(world, container);
        } else if (std::string(inner_node.name()) == "Minigun") {
            add_minigun(world, container);
        } else if (std::string(inner_node.name()) == "EnemyWeapon") {
            add_enemy_weapon(world, container);
        } else if (std::string(inner_node.name()) == "SmallCaliberAmmo") {
            add_small_caliber_ammo(world, container, inner_node);
        } else if (std::string(inner_node.name()) == "PistolAmmo") {
            add_pistol_ammo(world, container, inner_node);
        } else if (std::string(inner_node.name()) == "EnemyAmmo") {
            add_enemy_ammo(world, container, inner_node);
        }
    }
}

void add_inventory(flecs::world& world, pugi::xml_node& node, flecs::entity& entity) {
    flecs::entity container = world.entity().add<container::container_tag>();
    entity.add<container::inventory_tag>(container);

    add_container(world, node, container);
}

void add_coords(flecs::entity& entity, pugi::xml_node node) {
    float x = node.attribute("X").as_float();
    float y = node.attribute("Y").as_float();
    entity.set<movement::position>({x, y});
}

void entity_from_xml::init_player(pugi::xml_node& node, flecs::world& world) {
    flecs::entity player = world.entity().is_a<behavior::player_tag>();
    add_coords(player, node);
    add_inventory(world, node, player);
}

void entity_from_xml::init_aid_kit(pugi::xml_node& node, flecs::world& world) {
    flecs::entity aid_kit = world.entity().is_a<behavior::aid_kit_tag>();
    add_coords(aid_kit, node);
}

void entity_from_xml::init_loot_box(pugi::xml_node& node, flecs::world& world) {
    flecs::entity loot_box = world.entity().is_a<behavior::loot_box_tag>();
    add_coords(loot_box, node);
    add_container(world, node, loot_box);
}

void entity_from_xml::init_tnt_barrel(pugi::xml_node& node, flecs::world& world) {
    flecs::entity tnt_barrel = world.entity().is_a<behavior::tnt_barrel_tag>();
    add_coords(tnt_barrel, node);
}

void entity_from_xml::init_enemy(pugi::xml_node& node, flecs::world& world) {
    flecs::entity enemy = world.entity().is_a<behavior::zombie_prefab>();
    add_coords(enemy, node);
}

void entity_from_xml::init_static_turret(pugi::xml_node& node, flecs::world& world) {
    flecs::entity turret = world.entity().is_a<behavior::turret_tag>();
    add_coords(turret, node);

    add_inventory(world, node, turret);
}

void entity_from_xml::init_dynamic_turret(pugi::xml_node& node, flecs::world& world) {
    flecs::entity turret = world.entity().is_a<behavior::turret_tag>();
    float x1 = node.attribute("X1").as_float();
    float y1 = node.attribute("Y1").as_float();
    turret.set<movement::position>({x1, y1});
    turret.set<movement::position, movement::begin_tag>({x1, y1});

    float x2 = node.attribute("X2").as_float();
    float y2 = node.attribute("Y2").as_float();
    turret.set<movement::position, movement::end_tag>({x2, y2});

    add_inventory(world, node, turret);
}

void entity_from_xml::init_landmine(pugi::xml_node& node, flecs::world& world) {
    flecs::entity landmine = world.entity().is_a<behavior::landmine_prefab>();
    add_coords(landmine, node);
}

void entity_from_xml::load_levels(flecs::world& world) {
    pugi::xml_document doc;
    doc.load_file(PATH "/assets/config/config.xml");
    for (pugi::xml_node level : doc.children("Level")) {
        for (pugi::xml_node entity : level.children()) {
            if (std::string(entity.name()) == "Player") {
                init_player(entity, world);
            } else if (std::string(entity.name()) == "AidKit") {
                init_aid_kit(entity, world);
            } else if (std::string(entity.name()) == "Barrel") {
                init_tnt_barrel(entity, world);
            } else if (std::string(entity.name()) == "Enemy") {
                init_enemy(entity, world);
            } else if (std::string(entity.name()) == "StaticTurret") {
                init_static_turret(entity, world);
            } else if (std::string(entity.name()) == "DynamicTurret") {
                init_dynamic_turret(entity, world);
            } else if (std::string(entity.name()) == "LandMine") {
                init_landmine(entity, world);
            } else if (std::string(entity.name()) == "LootBox") {
                init_loot_box(entity, world);
            }
        }
    }
}
