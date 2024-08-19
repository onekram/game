#include "life.h"

#include <iostream>

void life::life_time_system(flecs::iter& it, std::size_t i, life_time& lt) {
    lt.time -= it.delta_time();

    if (lt.time < 0) {
        it.entity(i).add<destroy_tag>();
    }
}

void life::life_points_system(flecs::entity e, health_points& lp) {
    if (lp.points <= 0) {
        e.add<destroy_tag>();
    }
}

void life::destroy_entity_system(flecs::entity e) {
    e.destruct();
}

void life::init(flecs::world& world) {
    init_components<life_time, health_points, damage_points, destroy_tag>(world);

    world.system<life_time>("LifeTimeSystem").kind(flecs::OnValidate).each(life_time_system);

    world.system<health_points>("LifePointsSystem")
        .kind(flecs::OnValidate)
        .each(life_points_system);
    world.system<>().with<destroy_tag>().each(destroy_entity_system);
}
