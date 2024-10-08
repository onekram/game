#include "entity_spawn.h"

#include "behavior.h"
#include "container.h"
#include "global.h"

auto entity_spawn::enemy_spawn_system_factory(std::size_t count) {
    return [count](flecs::iter& it) {
        for (std::size_t i = 0; i < count; ++i) {
            auto pos = movement::generate_random_position(
                global::BORDER,
                global::WIDTH - global::BORDER,
                global::BORDER,
                global::HEIGHT - global::BORDER
            );
            it.world().entity().is_a<behavior::zombie_prefab>().set<movement::position>(pos);
        }
    };
}

void entity_spawn::player_spawn_system(flecs::iter& it) {
    it.world()
        .entity()
        .is_a<behavior::player_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .add<container::inventory_tag>(
            it.world().entity().add<container::container_tag>().with<container::contained_by_tag>(
                [&] {
                    it.world()
                        .entity()
                        .is_a<container::pistol_tag>()
                        .add<container::container_tag>()
                        .with<container::contained_by_tag>([&] {
                            it.world()
                                .entity()
                                .is_a<container::pistol_ammo_tag>()
                                .set<container::quantity>({10});
                        });
                    it.world()
                        .entity()
                        .is_a<container::automatic_weapon_tag>()
                        .add<container::container_tag>()
                        .with<container::contained_by_tag>([&] {
                            it.world()
                                .entity()
                                .is_a<container::small_caliber_ammo_tag>()
                                .set<container::quantity>({30});
                        });

                    it.world()
                        .entity()
                        .is_a<container::minigun_tag>()
                        .add<container::container_tag>()
                        .with<container::contained_by_tag>([&] {
                            it.world()
                                .entity()
                                .is_a<container::small_caliber_ammo_tag>()
                                .set<container::quantity>({1000});
                        });

                    it.world()
                        .entity()
                        .is_a<container::small_caliber_ammo_tag>()
                        .set<container::quantity>({1000});
                    it.world().entity().is_a<container::pistol_ammo_tag>().set<container::quantity>(
                        {100}
                    );
                }
            )
        );
}

void entity_spawn::aid_kid_spawn_system(flecs::iter& it) {
    it.world().entity().is_a<behavior::aid_kit_tag>().set<movement::position>(
        movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        )
    );
}

void entity_spawn::tnt_barrel_spawn_system(flecs::iter& it) {
    it.world().entity().is_a<behavior::tnt_barrel_tag>().set<movement::position>(
        movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        )
    );
}

void entity_spawn::landmine_spawn_system(flecs::iter& it) {
    it.world()
        .entity()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .is_a<behavior::landmine_prefab>();
}

void entity_spawn::loot_box_spawn_system(flecs::iter& it) {
    it.world()
        .entity()
        .is_a<behavior::loot_box_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .add<container::container_tag>()
        .with<container::contained_by_tag>([&] {
            it.world().entity().is_a<container::small_caliber_ammo_tag>().set<container::quantity>(
                {100}
            );
            it.world().entity().is_a<container::pistol_ammo_tag>().set<container::quantity>({100});
        });
}

void entity_spawn::static_turret_spawn_system(flecs::iter& it) {
    it.world()
        .entity()
        .is_a<behavior::turret_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .add<container::inventory_tag>(
            it.world().entity().add<container::container_tag>().with<container::contained_by_tag>(
                [&] {
                    it.world()
                        .entity()
                        .is_a<container::enemy_turret_tag>()
                        .add<container::container_tag>()
                        .add<container::active_tag>()
                        .with<container::contained_by_tag>([&] {
                            it.world()
                                .entity()
                                .is_a<container::enemy_ammo_tag>()
                                .set<container::quantity>({100});
                        });

                    it.world().entity().is_a<container::enemy_ammo_tag>().set<container::quantity>(
                        {10000}
                    );
                }
            )
        );
}

void entity_spawn::dynamic_turret_spawn_system(flecs::iter& it) {
    auto pos = movement::generate_random_position(
        global::BORDER,
        global::WIDTH - global::BORDER,
        global::BORDER,
        global::HEIGHT - global::BORDER
    );

    it.world()
        .entity()
        .is_a<behavior::turret_tag>()
        .set<movement::position>(pos)
        .set<movement::position, movement::begin_tag>(pos)
        .set<movement::position, movement::end_tag>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .add<container::inventory_tag>(
            it.world().entity().add<container::container_tag>().with<container::contained_by_tag>(
                [&] {
                    it.world()
                        .entity()
                        .is_a<container::enemy_turret_tag>()
                        .add<container::container_tag>()
                        .add<container::active_tag>()
                        .with<container::contained_by_tag>([&] {
                            it.world()
                                .entity()
                                .is_a<container::enemy_ammo_tag>()
                                .set<container::quantity>({100});
                        });

                    it.world().entity().is_a<container::enemy_ammo_tag>().set<container::quantity>(
                        {10000}
                    );
                }
            )
        );
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

    world.system("StaticTurretSpawnSystem")
        .kind(flecs::OnUpdate)
        .tick_source(each_second)
        .rate(10)
        .run(static_turret_spawn_system);

    world.system("DynamicTurretSpawnSystem")
        .kind(flecs::OnUpdate)
        .tick_source(each_second)
        .rate(20)
        .run(dynamic_turret_spawn_system);

    world.system("LandmineSpawnSystem")
        .kind(flecs::OnUpdate)
        .tick_source(each_second)
        .rate(20)
        .run(landmine_spawn_system);
}
