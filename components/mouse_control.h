#pragma once

#include "flecs.h"
#include "init_components.h"
#include "raylib.h"

namespace mouse_control {
struct mouse {
    float x;
    float y;

    bool pressed;
};

void mouse_control_system(mouse& m);

void init(flecs::world& world);
} // namespace mouse_control
