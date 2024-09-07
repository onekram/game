#include "shooting.h"

#include "behavior.h"
#include "container.h"
#include "distance.h"
#include "render.h"
#include "textures.h"

void shooting::handle_shoot_system(
    flecs::entity player,
    const movement::position& p,
    const mouse_control::mouse& mc
) {
    auto active = container::find_item_active(player);
    if (active.has<container::RangedWeapon>()) {
        if (active.has<container::automatic_tag>() && mc.down ||
            !active.has<container::automatic_tag>() && mc.click) {
            active.set<ShotDirection, Check>({p.x, p.y, mc.x, mc.y});
        }
    }
}

void shooting::bullet_spawn_system(
    flecs::iter& it,
    std::size_t i,
    const ShotDirection& sd,
    const container::attack_factor* k
) {
    auto weapon = it.entity(i);
    weapon.remove<ShotDirection>();
    auto bullet = container::get_cartridges_from_weapon(weapon);
    if (!bullet) {
        return;
    }
    float a = k ? k->k : 1;
    if (bullet.get<container::quantity>()->value > 0) {
        bullet.get_mut<container::quantity>()->value--;

        float length = global::BULLET_VELOCITY;
        float v_x = sd.dst_x - sd.src_x;
        float v_y = sd.dst_y - sd.src_y;
        float res = std::sqrt(v_x * v_x + v_y * v_y);
        it.world()
            .entity()
            .is_a(bullet)
            .set<life::damage_points>({bullet.get<life::damage_points>()->points * a})
            .remove<container::contained_by_tag>(flecs::Wildcard)
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
    auto component = e.target<behavior::aiming_at_tag>();
    float tx;
    float ty;
    bool init = false;
    e.world().query_builder<const movement::position>().with(component).build().each(
        [&tx, &ty, p, &init](const movement::position& target_pos) {
            if (!init || get_distance(p.x, p.y, target_pos.x, target_pos.y) <
                             get_distance(tx, ty, p.x, p.y)) {
                tx = target_pos.x;
                ty = target_pos.y;
                init = true;
            }
        }
    );
    if (!init) {
        return;
    }

    float dx = (tx - p.x);
    float dy = (ty - p.y);
    float distance = std::sqrt(dx * dx + dy * dy);
    if (distance < global::VISIBILITY_DISTANCE_TURRET) {
        auto active = container::find_item_active(e);
        if (active.has<container::RangedWeapon>()) {
            if (active.has<container::automatic_tag>()) {
                active.set<ShotDirection, Check>({p.x, p.y, tx, ty});
            }
        }
    }
}

void shooting::init(flecs::world& world) {
    world.system<const movement::position, const mouse_control::mouse>("HandleShotSystem")
        .with<container::inventory_tag>(flecs::Wildcard)
        .each(handle_shoot_system);

    world.system<const ShotDirection, const container::attack_factor*>("BulletSpawnSystem")
        .each(bullet_spawn_system);

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
        .with<container::inventory_tag>(flecs::Wildcard)
        .each(reloading_system);

    world.system<const movement::position>("HandleShotAimingSystem")
        .with<container::inventory_tag>(flecs::Wildcard)
        .with<behavior::aiming_at_tag>(flecs::Wildcard)
        .each(aiming_at_player_system);
}
