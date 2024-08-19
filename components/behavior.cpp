#include "behavior.h"

#include <iostream>

void behavior::handle_damage_system(flecs::entity e, life::damage_points& dp) {
    auto target = e.target<physical_interaction::interaction_tag>();
    auto component = e.target<can_damage_tag>();
    if (target.has(component)) {
        target.set<get_damage>({dp.points});
    }
}

void behavior::cause_damage_system(
    flecs::entity e,
    behavior::get_damage& gd,
    life::life_points& lp
) {
    lp.points -= gd.points;
    e.remove<behavior::get_damage>();
}

void behavior::init(flecs::world& world) {
    init_components<follow_tag, player_tag, enemy_tag, bullet_tag, get_damage, can_damage_tag>(world
    );

    world.system<life::damage_points>("HandleDamageSystemBullet")
        .kind(flecs::OnUpdate)
        .with<behavior::bullet_tag>()
        .with<can_damage_tag, enemy_tag>()
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .each(handle_damage_system);

    world.system<behavior::get_damage, life::life_points>("CauseDamageSystem")
        .kind(flecs::OnUpdate)
        .each(cause_damage_system);
}
