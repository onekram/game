#pragma once

#include "flecs.h"
#include "global.h"
#include "init_components.h"
#include "mouse_control.h"
#include "random_gen.h"
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

struct input_movement {
    bool up;
    bool down;
    bool left;
    bool right;
};

struct begin_tag {};

struct end_tag {};

velocity generate_random_velocity();
position generate_random_position(float MIN_X, float MAX_X, float MIN_Y, float MAX_Y);
input_movement get_default_input();

void velocity_input_system(velocity& v, const input_movement& i);
void change_velocity(velocity& v, float r_x, float r_y, float max_speed);
void move_system(flecs::iter& it, std::size_t i, position& p, velocity& v);
void move_bounce_system(position& p, velocity& v);
void velocity_follow_player_system(flecs::entity target_pos, const position& p, velocity& v);
void input_system(input_movement& input);
void path_movement(flecs::iter& it, std::size_t i, position& p, position& begin, position& end);
void init(flecs::world& world);
} // namespace movement
