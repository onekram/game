#pragma once

#include "flecs.h"
#include "global.h"
#include "init_components.h"
#include "random-gen.h"
#include "raylib.h"

namespace movement {
struct velocity {
    float x;
    float y;
};

struct position {
    float x;
    float y;
};

struct input {
    bool up;
    bool down;
    bool left;
    bool right;
};

struct player_tag {};

struct enemy_tag {};

struct follow_tag {};

velocity generate_random_velocity();

position generate_random_position(float MIN_X, float MAX_X, float MIN_Y, float MAX_Y);

input get_default_input();

void change_velocity(velocity& v, float r_x, float r_y);

void init(flecs::world& world);
} // namespace movement
