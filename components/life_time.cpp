#include "life_time.h"

#include <iostream>

void life_time::life_time_system(flecs::iter& it, std::size_t i, life_time& lt) {
    lt.time -= it.delta_time();

    if (lt.time < 0) {
        it.entity(i).destruct();
    }
}

void life_time::init(flecs::world& world) {
    init_components<life_time>(world);

    world.system<life_time>("LifeTimeSystem").each(life_time_system);
}
