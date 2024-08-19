#include "behavior.h"

#include <iostream>

void behavior::handle_damage_system(flecs::entity e, life::damage_points& dp) {
    auto target = e.target<physical_interaction::interaction_tag>();
    auto component = e.target<can_damage_tag>();
    if (target.has(component)) {
        target.set<get_damage>({dp.points});
        if (e.has<temporary_tag>()) {
            e.add<life::destroy_tag>();
        }
    }
}

void behavior::handle_health_restore_system(flecs::entity e, health_restore_points& hrp) {
    auto target = e.target<physical_interaction::interaction_tag>();
    auto component = e.target<can_restore_health_tag>();
    if (target.has(component)) {
        target.set<get_health>({hrp.points});
        if (e.has<temporary_tag>()) {
            e.add<life::destroy_tag>();
        }
    }
}

void behavior::cause_damage_system(
    flecs::entity e,
    behavior::get_damage& gd,
    life::health_points& lp
) {
    lp.points -= gd.points;
    e.remove<behavior::get_damage>();
}

void behavior::cause_health_restore_system(
    flecs::entity e,
    behavior::get_health& gh,
    life::health_points& lp
) {
    lp.points = std::min(lp.points + gh.points, lp.max);
    e.remove<behavior::get_health>();
}

void behavior::init(flecs::world& world) {
    init_components<
        follow_tag,
        player_tag,
        enemy_tag,
        aid_kit_tag,
        bullet_tag,
        get_damage,
        can_damage_tag,
        can_restore_health_tag,
        get_health,
        health_restore_points,
        temporary_tag>(world);

    world.system<life::damage_points>("HandleDamageSystemBullet")
        .kind(flecs::OnUpdate)
        .with<behavior::bullet_tag>()
        .with<can_damage_tag, enemy_tag>()
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .each(handle_damage_system);

    world.system<life::damage_points>("HandleDamageSystemEnemy")
        .kind(flecs::OnUpdate)
        .with<behavior::enemy_tag>()
        .with<can_damage_tag, player_tag>()
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .each(handle_damage_system);

    world.system<health_restore_points>("HandleHealthSystemAidKit")
        .kind(flecs::OnUpdate)
        .with<behavior::aid_kit_tag>()
        .with<can_restore_health_tag, player_tag>()
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .each(handle_health_restore_system);

    world.system<behavior::get_damage, life::health_points>("CauseDamageSystem")
        .kind(flecs::OnUpdate)
        .each(cause_damage_system);

    world.system<behavior::get_health, life::health_points>("CauseHealthRestoreSystem")
        .kind(flecs::OnUpdate)
        .each(cause_health_restore_system);
}
