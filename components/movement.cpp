#include "movement.h"

#include <iostream>

namespace movement {
velocity generate_random_velocity() {
    return {
        get_random(-global::MAX_SPEED / 3, global::MAX_SPEED / 3),
        get_random(-global::MAX_SPEED / 3, global::MAX_SPEED / 3)};
}

position generate_random_position(float MIN_X, float MAX_X, float MIN_Y, float MAX_Y) {
    return {get_random(MIN_X, MAX_X), get_random(MIN_Y, MAX_Y)};
}

input get_default_input() {
    return {false, false, false, false};
}

void change_velocity(velocity& v, float r_x, float r_y) {
    float n_x = v.x + r_x;
    float n_y = v.y + r_y;
    float res = std::sqrt(n_x * n_x + n_y * n_y);
    if (res <= global::MAX_SPEED) {
        v.x = n_x;
        v.y = n_y;
    } else {
        v.x = n_x * (global::MAX_SPEED / res);
        v.y = n_y * (global::MAX_SPEED / res);
    }
}

void velocity_input_system(velocity& v, const input& i) {
    if (i.right == i.left) {
        v.x /= 1.001;
        if (std::fabs(v.x) < global::MAX_SPEED / 10) {
            v.x = 0;
        }
    }
    if (i.up == i.down) {
        v.y /= 1.001;
        if (std::fabs(v.y) < global::MAX_SPEED / 10) {
            v.y = 0;
        }
    }
    change_velocity(v, (i.right - i.left) * global::MAX_SPEED / 1000, (i.down - i.up) * global::MAX_SPEED / 1000);
}

void move_system(position& p, velocity& v) {
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

void move_bounce_system(position& p, velocity& v) {
    if (p.x < global::BORDER || p.x > global::WIDTH - global::BORDER) {
        v.x = -v.x;
    }
    if (p.y < global::BORDER || p.y > global::HEIGHT - global::BORDER) {
        v.y = -v.y;
    }
    p.x += v.x;
    p.y += v.y;
}

void velocity_follow_player_system(flecs::entity e, const position& p, velocity& v) {
    auto target_pos = e.target<follow_tag>().get<position>();
    change_velocity(
        v,
        (target_pos->x > p.x ? 1.0f : -1.0f) * global::MAX_SPEED / 1000,
        (target_pos->y > p.y ? 1.0f : -1.0f) * global::MAX_SPEED / 1000
    );
}

void input_system(input& input) {
    input.up = IsKeyDown(KEY_W);
    input.down = IsKeyDown(KEY_S);
    input.left = IsKeyDown(KEY_A);
    input.right = IsKeyDown(KEY_D);
}

void repulsion(position& pos1, position& pos2, float dist, float k) {
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < dist) {
        float force = k / (distance + 0.1);
        pos1.x += (dx / distance) * force;
        pos1.y += (dy / distance) * force;
        pos2.x -= (dx / distance) * force;
        pos2.y -= (dy / distance) * force;
    }
}

void init(flecs::world& world) {
    init_components<position, velocity, input>(world);

    init_components<enemy_tag, player_tag, follow_tag>(world);

    world.system<position, velocity>("MovementSystem").each(move_system);

    world.system<velocity, input>("VelocityControlSystemPlayer").with<player_tag>().each(velocity_input_system);

    world.system<position, velocity>("VelocitySystemFollowingEnemy")
        .with<enemy_tag>()
        .with<follow_tag>(flecs::Wildcard)
        .each(velocity_follow_player_system);

    world.system<input>("InputSystemPlayer").with<player_tag>().each(input_system);

    world.system<position>("RepulsionEntitiesSystem").each([&world](flecs::entity e1, position& pos1) {
        world.each<position>([&](flecs::entity e2, position& pos2) {
            if (e1.id() != e2.id()) {
                repulsion(pos1, pos2, global::RADIUS_BALL + 2, 5);
            }
        });
    });
}
} // namespace movement
