#include "behavior.h"

#include "inventory/container.h"
#include "resources/sounds.h"

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
    flecs::iter& it,
    std::size_t i,
    behavior::get_damage& gd,
    life::health_points& lp
) {
    auto e = it.entity(i);
    auto target = e.target<behavior::get_damage>();

    if (target.has<time_between_damage>()) {
        auto tbd = target.get_mut<time_between_damage>();
        tbd->elapsed_time += it.delta_time();
        if (tbd->elapsed_time > tbd->required_time) {
            tbd->elapsed_time = 0;
        } else {
            return;
        }
    }
    lp.points -= gd.points;
    e.remove<behavior::get_damage>(target);
    target.add<life::already_use_tag>();
}

void behavior::cause_health_restore_system(
    flecs::entity e,
    behavior::get_health& gh,
    life::health_points& lp
) {
    auto target = e.target<behavior::get_health>();
    if (lp.points < lp.max) {
        lp.points = std::min(lp.points + gh.points, lp.max);
        target.add<life::already_use_tag>();
    }
    e.remove<behavior::get_health>(target);
}

void behavior::already_used_sound_system(flecs::iter& it, std::size_t i, const sound& s) {
    it.entity(i).remove<life::already_use_tag>();
    Sound sound = sounds::load_sound(s.sound);
    PlaySound(LoadSoundAlias(sound));
}

void behavior::handle_loot_box_system(flecs::iter& it, std::size_t i) {
    flecs::entity e = it.entity(i);
    flecs::entity target = it.pair(0).second();

    if (target.has<container::inventory_tag>(flecs::Wildcard)) {
        container::transfer_items(target, e);
        container::number_container_elements(target);
        e.add<life::already_use_tag>();
    }
}

void behavior::use_after_contact_system(flecs::iter& it, std::size_t i) {
    flecs::entity e = it.entity(i);
    flecs::entity target = it.pair(0).second();
    flecs::entity component = it.pair(1).second();

    if (target.has(component)) {
        e.add<life::destroy_tag>();
    }
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
        tnt_barrel_tag,
        destroy_animation_tag,
        loot_box_tag,
        sound,
        turret_tag,
        landmine_tag,
        use_after_contact_tag>(world);

    flecs::entity each_second = world.timer().interval(1.0);

    world.system<life::damage_points>("HandleDamageNotEnemySystem")
        .kind(flecs::OnUpdate)
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .with<can_damage_tag>(flecs::Wildcard)
        .without<enemy_tag>()
        .each(handle_damage_system);

    world.system<life::damage_points>("HandleDamageEnemySystem")
        .kind(flecs::OnUpdate)
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .with<can_damage_tag>(flecs::Wildcard)
        .with<enemy_tag>()
        .each(handle_damage_system);

    world.system<>("HandleLootBoxSystem")
        .kind(flecs::OnUpdate)
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .with<container::container_tag>()
        .with<loot_box_tag>()
        .each(handle_loot_box_system);

    world.system<>("HandleUseAfterContactSystem")
        .kind(flecs::OnUpdate)
        .with<physical_interaction::interaction_tag>(flecs::Wildcard)
        .with<use_after_contact_tag>(flecs::Wildcard)
        .each(use_after_contact_system);

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

    world.system<const behavior::sound>("SoundOnUsedSystem")
        .kind(flecs::OnUpdate)
        .with<life::already_use_tag>()
        .each(already_used_sound_system);
}
