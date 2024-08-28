#include "shooting.h"

#include "behavior.h"
#include "container.h"
#include "render.h"
#include "textures.h"

void shooting::handle_shoot_system(
    flecs::entity player,
    const movement::position& p,
    const mouse_control::mouse& mc
) {
    auto active = container::find_item_active(player);
    if (active.has<container::RangedWeapon>()) {
        if (active.has<container::Automatic>() && mc.down ||
            !active.has<container::Automatic>() && mc.click) {
            active.set<ShotDirection, Check>({p.x, p.y, mc.x, mc.y});
        }
    }
}

void shooting::bullet_spawn_system(flecs::iter& it, std::size_t i, const ShotDirection& sd) {
    auto weapon = it.entity(i);
    weapon.remove<ShotDirection>();
    auto bullet = container::get_cartridges_from_weapon(weapon);
    if (!bullet) {
        return;
    }
    if (bullet.get<container::Amount>()->value > 0) {
        bullet.get_mut<container::Amount>()->value--;

        float length = global::BULLET_VELOCITY;
        float v_x = sd.dst_x - sd.src_x;
        float v_y = sd.dst_y - sd.src_y;
        float res = std::sqrt(v_x * v_x + v_y * v_y);
        it.world()
            .entity()
            .is_a(bullet)
            .remove<container::ContainedBy>(flecs::Wildcard)
            .set<movement::position>({sd.src_x, sd.src_y})
            .set<movement::velocity>({v_x * length / res, v_y * length / res});
    }
}

void shooting::range_system(
    flecs::iter& it,
    std::size_t i,
    shooting::firing_range& fr,
    const movement::velocity& v
) {
    fr.value -= std::sqrt(v.x * v.x + v.y * v.y) * it.delta_time();
    if (fr.value <= 0) {
        it.entity(i).add<life::destroy_tag>();
    }
}

void shooting::reloading_system(flecs::entity container) {
    if (IsKeyDown(KEY_R)) {
        container::reloading_weapons(container);
    }
}

void shooting::time_between_shots_system(
    flecs::iter& it,
    std::size_t i,
    shooting::time_between_shots& tbs,
    const ShotDirection& sd
) {
    tbs.elapsed_time += it.delta_time();
    auto weapon = it.entity(i);
    if (tbs.elapsed_time > tbs.shot_time) {
        tbs.elapsed_time = 0;
        weapon.set<ShotDirection>({sd.src_x, sd.src_y, sd.dst_x, sd.dst_y});
        weapon.remove<ShotDirection, Check>();
    }
}

void shooting::shots_system(flecs::iter& it, std::size_t i, const ShotDirection& sd) {
    auto weapon = it.entity(i);
    weapon.set<ShotDirection>({sd.src_x, sd.src_y, sd.dst_x, sd.dst_y});
    weapon.remove<ShotDirection, Check>();
}

void shooting::aiming_at_player_system(flecs::entity e, const movement::position& p) {
    auto target_pos = e.target<behavior::aiming_at_tag>().get<movement::position>();
    float dx = (target_pos->x - p.x);
    float dy = (target_pos->y - p.y);
    float distance = std::sqrt(dx * dx + dy * dy);
    if (distance < global::VISIBILITY_DISTANCE_TURRET) {
        auto active = container::find_item_active(e);
        if (active.has<container::RangedWeapon>()) {
            if (active.has<container::Automatic>()) {
                active.set<ShotDirection, Check>({p.x, p.y, target_pos->x, target_pos->y});
            }
        }
    }
}

void shooting::init(flecs::world& world) {
    world.system<const movement::position, const mouse_control::mouse>("HandleShotSystem")
        .with<container::Inventory>(flecs::Wildcard)
        .each(handle_shoot_system);

    world.system<const ShotDirection>("BulletSpawnSystem").each(bullet_spawn_system);

    world.system<time_between_shots, const ShotDirection>("TimeBetweenShotsSystem")
        .term_at(1)
        .second<Check>()
        .each(time_between_shots_system);

    world.system<const ShotDirection>("ShotsSystem")
        .term_at(0)
        .second<Check>()
        .without<time_between_shots>()
        .each(shots_system);

    world.system<firing_range, const movement::velocity>("FiringRangeSystem").each(range_system);

    world.system<>("ReloadingSystem")
        .with<container::Inventory>(flecs::Wildcard)
        .each(reloading_system);

    world.system<const movement::position>("HandleShotAimingSystem")
        .with<container::Inventory>(flecs::Wildcard)
        .with<behavior::aiming_at_tag>(flecs::Wildcard)
        .each(aiming_at_player_system);
}
