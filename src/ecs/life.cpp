#include "life.h"

#include "behavior.h"
#include "render.h"
#include "textures.h"

#include <iostream>

void life::life_time_system(flecs::iter& it, std::size_t i, life_time& lt) {
    lt.time -= it.delta_time();

    if (lt.time < 0) {
        it.entity(i).add<destroy_tag>();
    }
}

void life::health_points_system(flecs::entity e, health_points& lp) {
    if (lp.points <= 0) {
        e.add<destroy_tag>();
    }
}

void life::destroy_entity_system(flecs::entity e) {
    e.destruct();
}

void life::destroy_action_explosion(flecs::entity e, const movement::position& p) {
    std::size_t total_frames = 15;
    float frame_swap_time = 0.15f;
    e.world()
        .entity()
        .add<behavior::explosion_tag>()
        .set<movement::position>({p.x, p.y})
        .set<render::sprite>(
            {0,
             total_frames,
             0,
             5,
             frame_swap_time,
             0,
             167,
             186.6,
             167,
             186.6,
             true,
             "../icons/explosion3.png"}
        )
        .set<life::life_time>({static_cast<float>(total_frames) * frame_swap_time})
        .set<damage_points>({10})
        .add<behavior::can_damage_tag, behavior::enemy_tag>()
        .add<behavior::can_damage_tag, behavior::player_tag>()
        .add<behavior::can_damage_tag, behavior::tnt_barrel_tag>()
        .set<physical_interaction::interaction_radius>({150})
        .set<physical_interaction::repulsion_radius>({1000, 100000});
    e.destruct();
}

void life::init(flecs::world& world) {
    init_components<
        life_time,
        health_points,
        damage_points,
        destroy_tag,
        temporary_tag,
        already_use_tag>(world);

    world.system<life_time>("LifeTimeSystem").kind(flecs::OnUpdate).each(life_time_system);

    world.system<health_points>("LifePointsSystem")
        .kind(flecs::OnValidate)
        .each(health_points_system);

    world.system<>("CheckAlreadyDoneSystem")
        .kind(flecs::OnValidate)
        .with<already_use_tag>()
        .with<temporary_tag>()
        .without<behavior::destroy_animation_tag>()
        .each(destroy_entity_system);

    world.system<>("EntityDestroySystem")
        .with<destroy_tag>()
        .without<behavior::destroy_animation_tag>()
        .each(destroy_entity_system);

    world.system<const movement::position>("ExplosionOnDestroySystem")
        .with<destroy_tag>()
        .with<behavior::destroy_animation_tag>()
        .with<behavior::tnt_barrel_tag>()
        .each(destroy_action_explosion);
}
