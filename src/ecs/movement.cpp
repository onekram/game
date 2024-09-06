#include "movement.h"

#include "behavior.h"
#include "distance.h"
#include "textures.h"

#include <iostream>

movement::velocity movement::generate_random_velocity() {
    return {
        get_random(-global::MAX_SPEED / 3, global::MAX_SPEED / 3),
        get_random(-global::MAX_SPEED / 3, global::MAX_SPEED / 3)
    };
}

movement::position
movement::generate_random_position(float MIN_X, float MAX_X, float MIN_Y, float MAX_Y) {
    return {get_random(MIN_X, MAX_X), get_random(MIN_Y, MAX_Y)};
}

movement::input_movement movement::get_default_input() {
    return {false, false, false, false};
}

void movement::change_velocity(velocity& v, float r_x, float r_y, float max_speed) {
    float n_x = v.x + r_x;
    float n_y = v.y + r_y;
    float res = std::sqrt(n_x * n_x + n_y * n_y);
    if (res <= max_speed) {
        v.x = n_x;
        v.y = n_y;
    } else {
        v.x = n_x * (max_speed / res);
        v.y = n_y * (max_speed / res);
    }
}

void movement::velocity_input_system(velocity& v, const input_movement& i) {
    if (i.right == i.left) {
        v.x /= 1.03;
        if (std::fabs(v.x) < global::MAX_SPEED / 100) {
            v.x = 0;
        }
    }
    if (i.up == i.down) {
        v.y /= 1.03;
        if (std::fabs(v.y) < global::MAX_SPEED / 100) {
            v.y = 0;
        }
    }
    change_velocity(
        v,
        (i.right - i.left) * global::MAX_SPEED,
        (i.down - i.up) * global::MAX_SPEED,
        global::MAX_SPEED
    );
}

void movement::move_system(flecs::iter& it, std::size_t i, position& p, velocity& v) {
    if (p.x + v.x < global::BORDER) {
        p.x = global::BORDER;
        v.x = 0;
    }
    if (p.x + v.x > global::WIDTH - global::BORDER) {
        p.x = global::WIDTH - global::BORDER;
        v.x = 0;
    }
    if (p.y + v.y < global::BORDER) {
        p.y = global::BORDER;
        v.y = 0;
    }
    if (p.y + v.y > global::HEIGHT - global::BORDER) {
        p.y = global::HEIGHT - global::BORDER;
        v.y = 0;
    }
    p.x += v.x;
    p.y += v.y;
}

void movement::move_bounce_system(position& p, velocity& v) {
    if (p.x < global::BORDER || p.x > global::WIDTH - global::BORDER) {
        v.x = -v.x;
    }
    if (p.y < global::BORDER || p.y > global::HEIGHT - global::BORDER) {
        v.y = -v.y;
    }
    p.x += v.x;
    p.y += v.y;
}

void movement::velocity_follow_player_system(flecs::entity e, const position& p, velocity& v) {
    auto component = e.target<behavior::follow_tag>();
    float tx;
    float ty;
    bool init = false;
    e.world().query_builder<const position>().with(component).build().each(
        [&tx, &ty, p, &init](const position& target_pos) {
            if (!init || get_distance(p.x, p.y, target_pos.x, target_pos.y) <
                             get_distance(tx, ty, p.x, p.y)) {
                tx = target_pos.x;
                ty = target_pos.y;
                init = true;
            }
        }
    );
    if (!init) {
        return;
    }
    change_velocity(
        v,
        (tx > p.x ? 1.0f : -1.0f) * global::MAX_SPEED / 10,
        (ty > p.y ? 1.0f : -1.0f) * global::MAX_SPEED / 10,
        global::MAX_SPEED / 2
    );
}

void movement::input_system(input_movement& input) {
    input.up = IsKeyDown(KEY_W);
    input.down = IsKeyDown(KEY_S);
    input.left = IsKeyDown(KEY_A);
    input.right = IsKeyDown(KEY_D);
}

void movement::path_movement(
    flecs::iter& it,
    std::size_t i,
    position& p,
    position& begin,
    position& end
) {
    float dx = end.x - begin.x;
    float dy = end.y - begin.y;

    float r = std::sqrt(dx * dx + dy * dy);

    p.x += (dx / r) * global::TURRET_VELOCITY;
    p.y += (dy / r) * global::TURRET_VELOCITY;
    if (std::sqrt((p.x - end.x) * (p.x - end.x) + (p.y - end.y) * (p.y - end.y)) <
        global::NEAR_DISTANCE) {
        std::swap(begin, end);
    }
}

void movement::init(flecs::world& world) {
    init_components<position, velocity, input_movement>(world);

    init_components<behavior::enemy_tag, behavior::player_tag, behavior::follow_tag>(world);

    world.system<position, velocity>("MovementSystem").each(move_system);

    world.system<velocity, input_movement>("VelocityControlSystem")
        .kind(flecs::PreUpdate)
        .each(velocity_input_system);

    world.system<position, velocity>("VelocitySystemFollowingEnemy")
        .with<behavior::enemy_tag>()
        .with<behavior::follow_tag>(flecs::Wildcard)
        .each(velocity_follow_player_system);

    world.system<input_movement>("InputMovementSystem").each(input_system);

    world.system<position, position, position>("PathMovementSystem")
        .term_at(1)
        .second<begin>()
        .term_at(2)
        .second<end>()
        .each(path_movement);
}
