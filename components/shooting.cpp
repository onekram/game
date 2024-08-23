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
            player.add<Shot>(active);
        }
    }
}

void shooting::shot_system(
    flecs::iter& it,
    std::size_t i,
    const mouse_control::mouse& m,
    const movement::position& p
) {
    auto weapon = it.entity(i).target<Shot>();
    it.entity(i).remove<Shot>(weapon);
    auto bullet = container::get_cartridges_from_weapon(weapon);
    if (!bullet) {
        return;
    }
    if (bullet.get<container::Amount>()->value > 0) {
        bullet.get_mut<container::Amount>()->value--;

        float length = global::BULLET_VELOCITY;
        float v_x = m.x - p.x;
        float v_y = m.y - p.y;
        float res = std::sqrt(v_x * v_x + v_y * v_y);
        it.world()
            .entity()
            .is_a(bullet)
            .remove<container::ContainedBy>(flecs::Wildcard)
            .set<movement::position>({p.x, p.y})
            .set<movement::velocity>({v_x * length / res, v_y * length / res})
            .set<render::sprite>(
                {0,
                 1,
                 0,
                 1,
                 0,
                 0,
                 748,
                 365,
                 748 / 27,
                 365 / 27,
                 true,
                 textures::load_texture("../icons/bullet.png")}
            );
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

void shooting::init(flecs::world& world) {
    world.system<mouse_control::mouse>("HandleShotSystem")
        .with<container::Inventory>(flecs::Wildcard)
        .each(handle_shoot_system);

    world.system<const mouse_control::mouse, const movement::position>("ShotSystem")
        .with<Shot>(flecs::Wildcard)
        .each(shot_system);

    world.system<firing_range, const movement::velocity>("FiringRangeSystem").each(range_system);
}
