#pragma once
#include "flecs.h"

template <typename... Types>
inline void init_components(const flecs::world& world) {
    (world.component<Types>(), ...);
}
