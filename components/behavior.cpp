#include "behavior.h"

#include <iostream>

void behavior::handle_damage_system(flecs::iter& it, std::size_t i, life::damage_points& dp) {
    flecs::entity e = it.entity(i);
    flecs::entity target = it.pair(1).second();
    flecs::entity component = it.pair(2).second();

    if (target.has(component)) {
        target.set<get_damage>(e, {dp.points});
    }
}

void behavior::handle_health_restore_system(
    flecs::iter& it,
    std::size_t i,
    health_restore_points& hrp
) {
    flecs::entity e = it.entity(i);
    flecs::entity target = it.pair(1).second();
    flecs::entity component = it.pair(2).second();

    if (target.has(component)) {
        target.set<get_health>(e, {hrp.points});
    }
}

void behavior::cause_damage_system(
    flecs::entity e,
    behavior::get_damage& gd,
    life::health_points& lp
) {
    auto target = e.target<behavior::get_damage>();
    lp.points -= gd.points;
    e.remove<behavior::get_damage>(target);
    target.add<life::already_done_tag>();
}

void behavior::cause_health_restore_system(
    flecs::entity e,
    behavior::get_health& gh,
    life::health_points& lp
) {
    auto target = e.target<behavior::get_health>();
    if (lp.points < lp.max) {
        lp.points = std::min(lp.points + gh.points, lp.max);
        target.add<life::already_done_tag>();
    }
    e.remove<behavior::get_health>(target);
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
        tnt_barrel_tag>(world);

    world.system<life::damage_points>("HandleDamageSystem")
        .kind(flecs::OnUpdate)
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .with<can_damage_tag>(flecs::Wildcard)
        .each(handle_damage_system);

    world.system<health_restore_points>("HandleHealthSystem")
        .kind(flecs::OnUpdate)
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .with<can_restore_health_tag, player_tag>()
        .each(handle_health_restore_system);

    world.system<behavior::get_damage, life::health_points>("CauseDamageSystem")
        .term_at(0)
        .second(flecs::Wildcard)
        .kind(flecs::OnUpdate)
        .each(cause_damage_system);

    world.system<behavior::get_health, life::health_points>("CauseHealthRestoreSystem")
        .term_at(0)
        .second(flecs::Wildcard)
        .kind(flecs::OnUpdate)
        .each(cause_health_restore_system);
}
