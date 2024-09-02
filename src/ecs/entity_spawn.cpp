#include "entity_spawn.h"

#include "behavior.h"
#include "container.h"
#include "global.h"
#include "render.h"
#include "sounds.h"
#include "textures.h"

auto entity_spawn::enemy_spawn_system_factory(std::size_t count) {
    return [count](flecs::iter& it) {
        auto player = it.world().lookup("Player");
        for (std::size_t i = 0; i < count; ++i) {
            it.world()
                .entity()
                .add<behavior::enemy_tag>()
                .set<movement::position>(movement::generate_random_position(
                    global::BORDER,
                    global::WIDTH - global::BORDER,
                    global::BORDER,
                    global::HEIGHT - global::BORDER
                ))
                .add<movement::velocity>()
                .set<render::sprite>(
                    {0, 3, 2, 3, 32.6f, 47.0f, 17.0f * 3, 25.0f * 3, true, "../icons/zombie.png"}
                )
                .set<render::sprite_swap>({0.3f, 0})
                .set<life::health_points>({global::ENEMY_LIFE_POINTS, global::ENEMY_LIFE_POINTS})
                .set<life::damage_points>({10})
                .add<behavior::follow_tag>(player)
                .add<behavior::can_damage_tag, behavior::player_tag>()
                .set<physical_interaction::repulsion_radius>({20, 1})
                .set<physical_interaction::interaction_radius>({35})
                .set<behavior::sound>({"../sounds/zombie_attack.wav"})
                .set<behavior::time_between_damage>({0, 1});
        }
    };
}

void entity_spawn::player_spawn_system(flecs::iter& it) {
    it.world()
        .entity("Player")
        .add<behavior::player_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .set<movement::velocity>({0, 0})
        .set<movement::input_movement>(movement::get_default_input())
        .set<render::sprite>(
            {0, 2, 3, 3, 376.0f, 355.0f, 37.0f * 2, 35.0f * 2, true, "../icons/pngegg.png"}
        )
        .set<render::sprite_swap>({0.2f, 0})
        .set<mouse_control::mouse>({0, 0})
        .set<life::health_points>({global::PLAYER_LIFE_POINTS, global::PLAYER_LIFE_POINTS})
        .set<physical_interaction::repulsion_radius>({35, 2})
        .set<physical_interaction::interaction_radius>({30})
        .add<container::Inventory>(
            it.world().entity().add<container::Container>().with<container::ContainedBy>([&] {
                it.world()
                    .entity()
                    .is_a<container::Gun>()
                    .add<container::Active>()
                    .add<container::Container>()
                    .with<container::ContainedBy>([&] {
                        it.world().entity().is_a<container::PistolAmmo>().set<container::Amount>({10
                        });
                    });
                it.world()
                    .entity()
                    .is_a<container::AutomaticWeapon>()
                    .add<container::Container>()
                    .with<container::ContainedBy>([&] {
                        it.world()
                            .entity()
                            .is_a<container::SmallCaliberAmmo>()
                            .set<container::Amount>({30});
                    });

                it.world()
                    .entity()
                    .is_a<container::Minigun>()
                    .add<container::Container>()
                    .with<container::ContainedBy>([&] {
                        it.world()
                            .entity()
                            .is_a<container::SmallCaliberAmmo>()
                            .set<container::Amount>({1000});
                    });

                it.world().entity().is_a<container::SmallCaliberAmmo>().set<container::Amount>({1000
                });
                it.world().entity().is_a<container::PistolAmmo>().set<container::Amount>({100});
            })
        );
}

void entity_spawn::aid_kid_spawn_system(flecs::iter& it) {
    it.world()
        .entity()
        .add<behavior::aid_kit_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .set<render::sprite>(
            {0, 1, 1, 1, 596.0f, 626.0f, 596.0f / 14, 626.0f / 14, true, "../icons/aid_kit.png"}
        )
        .add<behavior::can_restore_health_tag, behavior::player_tag>()
        .set<behavior::health_restore_points>({200})
        .add<life::temporary_tag>()
        .set<physical_interaction::interaction_radius>({30});
}

void entity_spawn::tnt_barrel_spawn_system(flecs::iter& it) {
    it.world()
        .entity()
        .add<behavior::tnt_barrel_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .set<render::sprite>(
            {0, 1, 0, 1, 300.0f, 300.0f, 300.0f / 6, 300.0f / 6, true, "../icons/barrel.png"}
        )
        .set<life::health_points>({50, 50})
        .set<physical_interaction::interaction_radius>({50})
        .add<behavior::destroy_animation_tag>();
}

void entity_spawn::loot_box_spawn_system(flecs::iter& it) {
    it.world()
        .entity()
        .add<behavior::loot_box_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .set<render::sprite>(
            {0, 1, 0, 1, 332.0f, 200.0f, 332.0f / 7, 200.0f / 7, true, "../icons/ammo_loot.png"}
        )
        .set<physical_interaction::interaction_radius>({40})
        .add<container::Container>()
        .with<container::ContainedBy>([&] {
            it.world().entity().is_a<container::SmallCaliberAmmo>().set<container::Amount>({100});
            it.world().entity().is_a<container::PistolAmmo>().set<container::Amount>({100});
        })
        .add<life::temporary_tag>();
}

void entity_spawn::turret_spawn_system(flecs::iter& it) {
    auto player = it.world().lookup("Player");
    it.world()
        .entity()
        .add<behavior::enemy_tag>()
        .add<behavior::turret_tag>()
        .set<life::health_points>({1000, 1000})
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .set<physical_interaction::interaction_radius>({global::RADIUS_BALL})
        .add<container::Inventory>(
            it.world().entity().add<container::Container>().with<container::ContainedBy>([&] {
                it.world()
                    .entity()
                    .is_a<container::EnemyTurret>()
                    .add<container::Container>()
                    .add<container::Active>()
                    .with<container::ContainedBy>([&] {
                        it.world().entity().is_a<container::EnemyAmmo>().set<container::Amount>({100
                        });
                    });

                it.world().entity().is_a<container::EnemyAmmo>().set<container::Amount>({10000});
            })
        )
        .add<behavior::aiming_at_tag>(player)
        .set<render::sprite>(
            {0, 2, 8, 3, 720 / 3, 720 / 3, 720 / 6, 720 / 6, true, "../icons/turret.png"}
        )
        .add<render::rotation>();
}

void entity_spawn::init(flecs::world& world) {
    flecs::entity each_second = world.timer().interval(1.0);

    world.system("InitPlayerSystem").kind(flecs::OnStart).run(player_spawn_system);

    world.system("EnemyInitSystem").kind(flecs::OnStart).run(enemy_spawn_system_factory(10));

    world.system("EnemySpawnSystem")
        .kind(flecs::OnUpdate)
        .tick_source(each_second)
        .rate(10)
        .run(enemy_spawn_system_factory(4));

    world.system("AidKitSpawnSystem")
        .kind(flecs::OnUpdate)
        .tick_source(each_second)
        .rate(20)
        .run(aid_kid_spawn_system);

    world.system("TNTBarrelSpawnSystem")
        .kind(flecs::OnUpdate)
        .tick_source(each_second)
        .rate(4)
        .run(tnt_barrel_spawn_system);

    world.system("LootBoxSpawnSystem")
        .kind(flecs::OnUpdate)
        .tick_source(each_second)
        .rate(15)
        .run(loot_box_spawn_system);

    world.system("TurretSpawnSystem")
        .kind(flecs::OnUpdate)
        .tick_source(each_second)
        .rate(10)
        .run(turret_spawn_system);
}
