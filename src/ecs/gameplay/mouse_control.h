#pragma once

#include "flecs.h"
#include "raylib.h"
#include "utils/init_components.h"

namespace mouse_control {
struct mouse {
    float x;
    float y;

    bool click;
    bool down;
    bool release;

    float scroll_amount;
};

void mouse_control_system(mouse& m);

void init(flecs::world& world);
} // namespace mouse_control
