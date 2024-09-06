#include "load_prefabs.h"

#include "behavior.h"
#include "container.h"
#include "life.h"
#include "render.h"
#include "shooting.h"

#include <iostream>

void init_inventory(flecs::world& world) {
    world.prefab<container::AutomaticWeapon>()
        .add<container::RangedWeapon>()
        .add<container::CanHold>()
        .add<container::Automatic>()
        .set<container::MagazineSize>({30})
        .add<container::LoadedWith, container::SmallCaliberAmmo>()
        .set_auto_override<shooting::time_between_shots>({0, 0.15})
        .set<container::AttackCoef>({2});

    world.prefab<container::Minigun>()
        .add<container::RangedWeapon>()
        .add<container::CanHold>()
        .add<container::Automatic>()
        .set<container::MagazineSize>({1000})
        .add<container::LoadedWith, container::SmallCaliberAmmo>()
        .set_auto_override<shooting::time_between_shots>({0, 0.07})
        .set<container::AttackCoef>({0.5f});

    world.prefab<container::Pistol>()
        .add<container::RangedWeapon>()
        .add<container::CanHold>()
        .set<container::MagazineSize>({10})
        .add<container::LoadedWith, container::PistolAmmo>()
        .set<container::AttackCoef>({3});

    world.prefab<container::EnemyTurret>()
        .add<container::RangedWeapon>()
        .add<container::CanHold>()
        .set<container::MagazineSize>({100})
        .add<container::LoadedWith, container::EnemyAmmo>()
        .add<container::Automatic>()
        .set_auto_override<shooting::time_between_shots>({0, 0.6});

    world.prefab<container::SmallCaliberAmmo>()
        .add<container::Ammo>()
        .set<life::damage_points>({20})
        .add<behavior::bullet_tag>()
        .add<behavior::can_damage_tag, behavior::enemy_tag>()
        .add<behavior::can_damage_tag, behavior::tnt_barrel_tag>()
        .add<life::temporary_tag>()
        .set_auto_override<shooting::firing_range>({5})
        .add<render::sprite_angle>()
        .set_auto_override<physical_interaction::interaction_radius>({1})
        .set<render::sprite>(
            {0,
             1,
             0,
             1,
             748,
             365,
             748 / 50,
             365 / 50,
             true,
             "/home/onekram/CLionProjects/game/icons/bullet.png"}
        );

    world.prefab<container::PistolAmmo>()
        .add<container::Ammo>()
        .set<life::damage_points>({30})
        .add<behavior::bullet_tag>()
        .add<behavior::can_damage_tag, behavior::enemy_tag>()
        .add<behavior::can_damage_tag, behavior::tnt_barrel_tag>()
        .add<life::temporary_tag>()
        .set_auto_override<shooting::firing_range>({7})
        .add<render::sprite_angle>()
        .set_auto_override<physical_interaction::interaction_radius>({3})
        .set<render::sprite>(
            {0,
             1,
             0,
             1,
             748,
             365,
             748 / 27,
             365 / 27,
             true,
             "/home/onekram/CLionProjects/game/icons/bullet.png"}
        );

    world.prefab<container::EnemyAmmo>()
        .add<container::Ammo>()
        .set<life::damage_points>({30})
        .add<behavior::bullet_tag>()
        .add<behavior::can_damage_tag, behavior::player_tag>()
        .add<behavior::can_damage_tag, behavior::tnt_barrel_tag>()
        .add<life::temporary_tag>()
        .set_auto_override<shooting::firing_range>({10})
        .add<render::sprite_angle>()
        .set_auto_override<physical_interaction::interaction_radius>({3})
        .set<render::sprite>(
            {0,
             1,
             0,
             1,
             541,
             276,
             541 / 20,
             276 / 20,
             true,
             "/home/onekram/CLionProjects/game/icons/bullet_fire.png"}
        );
}

void init_player(flecs::world& world) {
    world.prefab<behavior::player_tag>()
        .add<behavior::player_tag>()
        .set<movement::velocity>({0, 0})
        .set<movement::input_movement>(movement::get_default_input())
        .set<render::sprite>(
            {0,
             2,
             3,
             3,
             376.0f,
             355.0f,
             37.0f * 2,
             35.0f * 2,
             true,
             "/home/onekram/CLionProjects/game/icons/pngegg.png"}
        )
        .set<render::sprite_swap>({0.2f, 0})
        .set<mouse_control::mouse>({0, 0})
        .set_auto_override<life::health_points>(
            {global::PLAYER_LIFE_POINTS, global::PLAYER_LIFE_POINTS}
        )
        .set<physical_interaction::repulsion_radius>({35, 2})
        .set<physical_interaction::interaction_radius>({30})
        .add<container::Inventory>(
            world.entity().add<container::Container>().with<container::ContainedBy>([&] {
                world.entity()
                    .is_a<container::Pistol>()
                    .add<container::Active>()
                    .add<container::Container>()
                    .with<container::ContainedBy>([&] {
                        world.entity().is_a<container::PistolAmmo>().set<container::Amount>({10});
                    });
                world.entity()
                    .is_a<container::AutomaticWeapon>()
                    .add<container::Container>()
                    .with<container::ContainedBy>([&] {
                        world.entity().is_a<container::SmallCaliberAmmo>().set<container::Amount>(
                            {30}
                        );
                    });

                world.entity()
                    .is_a<container::Minigun>()
                    .add<container::Container>()
                    .with<container::ContainedBy>([&] {
                        world.entity().is_a<container::SmallCaliberAmmo>().set<container::Amount>(
                            {1000}
                        );
                    });

                world.entity().is_a<container::SmallCaliberAmmo>().set<container::Amount>({1000});
                world.entity().is_a<container::PistolAmmo>().set<container::Amount>({100});
            })
        );
}

void init_aid_kit(flecs::world& world) {
    world.prefab<behavior::aid_kit_tag>()
        .add<behavior::aid_kit_tag>()
        .set<render::sprite>(
            {0,
             1,
             1,
             1,
             596.0f,
             626.0f,
             596.0f / 14,
             626.0f / 14,
             true,
             "/home/onekram/CLionProjects/game/icons/aid_kit.png"}
        )
        .add<behavior::can_restore_health_tag, behavior::player_tag>()
        .set<behavior::health_restore_points>({200})
        .add<life::temporary_tag>()
        .set<physical_interaction::interaction_radius>({30});
}

void init_landmine(flecs::world& world) {
    world.prefab<behavior::Landmine>()
        .add<behavior::landmine_tag>()
        .set<render::sprite>(
            {0,
             1,
             1,
             1,
             129.0f,
             96.0f,
             129.0f / 6,
             96.0f / 6,
             true,
             "/home/onekram/CLionProjects/game/icons/mine.png"}
        )
        .set<physical_interaction::interaction_radius>({20})
        .add<behavior::destroy_animation_tag>()
        .add<behavior::use_after_contact_tag, behavior::player_tag>();
}

void init_enemy(flecs::world& world) {
    world.prefab<behavior::Zombie>()
        .add<behavior::enemy_tag>()
        .set<movement::velocity>({0, 0})
        .set<render::sprite>(
            {0,
             3,
             2,
             3,
             32.6f,
             47.0f,
             17.0f * 3,
             25.0f * 3,
             true,
             "/home/onekram/CLionProjects/game/icons/zombie.png"}
        )
        .set<render::sprite_swap>({0.3f, 0})
        .set<life::health_points>({global::ENEMY_LIFE_POINTS, global::ENEMY_LIFE_POINTS})
        .set<life::damage_points>({10})
        .add<behavior::follow_tag, behavior::player_tag>()
        .add<behavior::can_damage_tag, behavior::player_tag>()
        .set<physical_interaction::repulsion_radius>({20, 1})
        .set<physical_interaction::interaction_radius>({35})
        .set<behavior::sound>({"/home/onekram/CLionProjects/game/sounds/zombie_attack.wav"})
        .set<behavior::time_between_damage>({0, 1});
}

void init_tnt_barrel(flecs::world& world) {
    world.prefab<behavior::tnt_barrel_tag>()
        .add<behavior::tnt_barrel_tag>()
        .set<render::sprite>(
            {0,
             1,
             1,
             1,
             300.0f,
             300.0f,
             300.0f / 6,
             300.0f / 6,
             true,
             "/home/onekram/CLionProjects/game/icons/barrel.png"}
        )
        .set<life::health_points>({50, 50})
        .set<physical_interaction::interaction_radius>({50})
        .add<behavior::destroy_animation_tag>();
}

void init_loot_box(flecs::world& world) {
    world.prefab<behavior::loot_box_tag>()
        .add<behavior::loot_box_tag>()
        .set<render::sprite>(
            {0,
             1,
             0,
             1,
             332.0f,
             200.0f,
             332.0f / 7,
             200.0f / 7,
             true,
             "/home/onekram/CLionProjects/game/icons/ammo_loot.png"}
        )
        .set<physical_interaction::interaction_radius>({40})
        .add<container::Container>()
        .with<container::ContainedBy>([&] {
            world.entity().is_a<container::SmallCaliberAmmo>().set<container::Amount>({100});
            world.entity().is_a<container::PistolAmmo>().set<container::Amount>({100});
        })
        .add<life::temporary_tag>();
}

void init_static_turret(flecs::world& world) {
    world.prefab<behavior::turret_tag>()
        .add<behavior::enemy_tag>()
        .add<behavior::turret_tag>()
        .set<life::health_points>({1000, 1000})
        .set<physical_interaction::interaction_radius>({30})
        .add<container::Inventory>(
            world.entity().add<container::Container>().with<container::ContainedBy>([&] {
                world.entity()
                    .is_a<container::EnemyTurret>()
                    .add<container::Container>()
                    .add<container::Active>()
                    .with<container::ContainedBy>([&] {
                        world.entity().is_a<container::EnemyAmmo>().set<container::Amount>({100});
                    });

                world.entity().is_a<container::EnemyAmmo>().set<container::Amount>({10000});
            })
        )
        .add<behavior::aiming_at_tag, behavior::player_tag>()
        .set<render::sprite>(
            {0,
             2,
             8,
             3,
             720 / 3,
             720 / 3,
             720 / 6,
             720 / 6,
             true,
             "/home/onekram/CLionProjects/game/icons/turret.png"}
        )
        .add<render::rotation>();
}

void init_explosion(flecs::world& world) {
    std::size_t total_frames = 15;
    float frame_swap_time = 0.15f;
    world.prefab<behavior::explosion_tag>()
        .add<behavior::explosion_tag>()
        .set<render::sprite>(
            {0,
             0,
             total_frames,
             5,
             167,
             186.6,
             167,
             186.6,
             true,
             "/home/onekram/CLionProjects/game/icons/explosion3.png"}
        )
        .set<render::sprite_swap>({frame_swap_time, 0})
        .set<life::life_time>({static_cast<float>(total_frames) * frame_swap_time})
        .set<life::damage_points>({7})
        .add<behavior::can_damage_tag, behavior::enemy_tag>()
        .add<behavior::can_damage_tag, behavior::player_tag>()
        .add<behavior::can_damage_tag, behavior::tnt_barrel_tag>()
        .set<physical_interaction::interaction_radius>({150})
        .set<physical_interaction::repulsion_radius>({1000, 100000});
}

void load_prefabs::init(flecs::world& world) {
    world.component<container::ContainedBy>().add(flecs::Exclusive);
    world.component<container::Container>().add(flecs::OnInstantiate, flecs::Override);

    world.component<container::RangedWeapon>().is_a<container::Item>();
    world.component<container::Ammo>().is_a<container::Item>();

    init_inventory(world);
    init_player(world);
    init_aid_kit(world);
    init_landmine(world);
    init_enemy(world);
    init_tnt_barrel(world);
    init_loot_box(world);
    init_static_turret(world);
    init_explosion(world);
}