#include "shooting.h"

#include "behavior.h"
#include "container.h"
#include "render.h"
#include "textures.h"

void shooting::handle_shoot_system(flecs::entity player, mouse_control::mouse& mc) {
    auto active = container::find_item_active(player);
    if (active.has<container::RangedWeapon>()) {
        if (active.has<container::Automatic>() && mc.down ||
            !active.has<container::Automatic>() && mc.click) {
            active.add<Shot>(player);
        }
    }
}

void shooting::bullet_spawn_system(flecs::iter& it, std::size_t i) {
    auto weapon = it.entity(i);
    auto from = weapon.target<ShotFrom>();
    weapon.remove<ShotFrom>(from);
    auto bullet = container::get_cartridges_from_weapon(weapon);
    if (!bullet) {
        return;
    }
    auto p = from.get<movement::position>();
    auto m = from.get<mouse_control::mouse>();
    if (bullet.get<container::Amount>()->value > 0) {
        bullet.get_mut<container::Amount>()->value--;

        float length = global::BULLET_VELOCITY;
        float v_x = m->x - p->x;
        float v_y = m->y - p->y;
        float res = std::sqrt(v_x * v_x + v_y * v_y);
        it.world()
            .entity()
            .is_a(bullet)
            .remove<container::ContainedBy>(flecs::Wildcard)
            .set<movement::position>({p->x, p->y})
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
    shooting::time_between_shots& tbs
) {
    tbs.elapsed_time += it.delta_time();
    auto weapon = it.entity(i);
    auto target = it.pair(1).second();
    if (tbs.elapsed_time > tbs.shot_time) {
        tbs.elapsed_time = 0;
        weapon.add<ShotFrom>(target);
        weapon.remove<Shot>(target);
    }
}

void shooting::shots_system(flecs::iter& it, std::size_t i) {
    auto weapon = it.entity(i);
    auto target = it.pair(0).second();
    weapon.add<ShotFrom>(target);
    weapon.remove<Shot>(target);
}

void shooting::init(flecs::world& world) {
    world.system<mouse_control::mouse>("HandleShotSystem")
        .with<container::Inventory>(flecs::Wildcard)
        .each(handle_shoot_system);

    world.system<>("BulletSpawnSystem").with<ShotFrom>(flecs::Wildcard).each(bullet_spawn_system);

    world.system<time_between_shots>("TimeBetweenShotsSystem")
        .with<Shot>(flecs::Wildcard)
        .each(time_between_shots_system);

    world.system("ShotsSystem")
        .with<Shot>(flecs::Wildcard)
        .without<time_between_shots>()
        .each(shots_system);

    world.system<firing_range, const movement::velocity>("FiringRangeSystem").each(range_system);

    world.system<>("ReloadingSystem")
        .with<container::Inventory>(flecs::Wildcard)
        .each(reloading_system);
}
