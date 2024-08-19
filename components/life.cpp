#include "life.h"

#include <iostream>

void life::life_time_system(flecs::iter& it, std::size_t i, life_time& lt) {
    lt.time -= it.delta_time();

    if (lt.time < 0) {
        it.entity(i).destruct();
    }
}

void life::life_points_system(flecs::entity e, life_points& lp) {
    if (lp.points <= 0) {
        e.destruct();
    }
}

void life::init(flecs::world& world) {
    init_components<life_time, life_points, damage_points>(world);

    world.system<life_time>("LifeTimeSystem").kind(flecs::OnValidate).each(life_time_system);

    world.system<life_points>("LifePointsSystem").kind(flecs::OnValidate).each(life_points_system);
}
